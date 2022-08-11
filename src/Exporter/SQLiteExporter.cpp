#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/ColDef.h"
#include "DB/DBTool.h"
#include "DB/SQLiteFile.h"
#include "DB/ReadingDB.h"
#include "DB/ReadingDBTool.h"
#include "Exporter/SQLiteExporter.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Writer.h"

Exporter::SQLiteExporter::SQLiteExporter()
{
}

Exporter::SQLiteExporter::~SQLiteExporter()
{
}

Int32 Exporter::SQLiteExporter::GetName()
{
	return *(Int32*)"SQLI";
}

IO::FileExporter::SupportType Exporter::SQLiteExporter::IsObjectSupported(IO::ParsedObject *pobj)
{
	if (pobj->GetParserType() != IO::ParserType::ReadingDB && pobj->GetParserType() != IO::ParserType::MapLayer)
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	return IO::FileExporter::SupportType::PathOnly;
}

Bool Exporter::SQLiteExporter::GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcatC(nameBuff, UTF8STRC("SQLite File"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.sqlite"));
		return true;
	}
	return false;
}

Bool Exporter::SQLiteExporter::ExportFile(IO::SeekableStream *stm, Text::CString fileName, IO::ParsedObject *pobj, void *param)
{
	if (pobj->GetParserType() != IO::ParserType::ReadingDB)
	{
		return false;
	}
	IO::Path::DeleteFile(fileName.v);
	DB::DBTool *destDB;
	IO::LogTool log;
	DB::ReadingDB *sDB;
	DB::ReadingDBTool *srcDB;
	DB::DBReader *r;
	DB::TableDef *tabDef;
	Data::ArrayList<Text::String*> tables;
	UOSInt i;
	UOSInt j;
	OSInt k;
	destDB = DB::SQLiteFile::CreateDBTool(fileName, &log, CSTR("DB: "));
	if (destDB == 0)
		return false;
	Bool succ = true;
	DB::SQLBuilder sql(destDB);
	Text::StringBuilderUTF8 sb;
	sDB = (DB::ReadingDB *)pobj;
	sDB->QueryTableNames(CSTR_NULL, &tables);
	if (sDB->IsFullConn())
	{
		NEW_CLASS(srcDB, DB::ReadingDBTool((DB::DBConn*)sDB, false, &log, CSTR("SDB: ")));
	}
	else
	{
		srcDB = 0;
	}
	i = 0;
	j = tables.GetCount();
	while (i < j)
	{
		if (srcDB)
		{
			tabDef = srcDB->GetTableDef(CSTR_NULL, tables.GetItem(i)->ToCString());
			if (tabDef)
			{
				r = srcDB->QueryTableData(CSTR_NULL, tables.GetItem(i)->ToCString(), 0, 0, 0, CSTR_NULL, 0);
				if (r == 0)
				{
					DEL_CLASS(tabDef);
					tabDef = 0;
				}
			}
			else
			{
				DEL_CLASS(tabDef);
				tabDef = 0;
				r = 0;
			}
		}
		else
		{
			r = sDB->QueryTableData(CSTR_NULL, tables.GetItem(i)->ToCString(), 0, 0, 0, CSTR_NULL, 0);
			if (r)
			{
				tabDef = r->GenTableDef(tables.GetItem(i)->ToCString());
			}
			else
			{
				tabDef = 0;
			}
		}
		if (r)
		{
			sql.Clear();
			DB::DBTool::GenCreateTableCmd(&sql, CSTR_NULL, tables.GetItem(i)->ToCString(), tabDef);
			if (destDB->ExecuteNonQuery(sql.ToCString()) <= -2)
			{
				{
					IO::FileStream debugFS(CSTR("Debug.txt"), IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
					Text::UTF8Writer debugWriter(&debugFS);
					debugWriter.WriteLineCStr(sql.ToCString());
				}

				DEL_CLASS(tabDef);
				succ = false;
				destDB->EndTrans(true);
				sDB->CloseReader(r);
				break;
			}
			DEL_CLASS(tabDef);
			destDB->BeginTrans();
			k = 10000;
			while (r->ReadNext())
			{
				if (k-- <= 0)
				{
					destDB->EndTrans(true);
					destDB->BeginTrans();
					k = 10000;
				}
				sql.Clear();
				DB::DBTool::GenInsertCmd(&sql, CSTR_NULL, tables.GetItem(i)->ToCString(), r);
				if (destDB->ExecuteNonQuery(sql.ToCString()) <= 0)
				{
					sb.ClearStr();
					destDB->GetLastErrorMsg(&sb);

					IO::FileStream debugFS(CSTR("Debug.txt"), IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
					Text::UTF8Writer debugWriter(&debugFS);
					debugWriter.WriteLineCStr(sql.ToCString());
					debugWriter.WriteLineCStr(sb.ToCString());
					succ = false;
					break;
				}
			}
			destDB->EndTrans(true);
			sDB->CloseReader(r);
		}
		else
		{
			succ = false;
			break;
		}

		if (!succ)
		{
			break;
		}
		i++;
	}

	LIST_FREE_STRING(&tables);
	SDEL_CLASS(srcDB);
	DEL_CLASS(destDB);
	return succ;
}
