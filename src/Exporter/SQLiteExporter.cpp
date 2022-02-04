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

Bool Exporter::SQLiteExporter::ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param)
{
	if (pobj->GetParserType() != IO::ParserType::ReadingDB)
	{
		return false;
	}
	IO::Path::DeleteFile(fileName);
	DB::DBTool *destDB;
	IO::LogTool log;
	DB::ReadingDB *sDB;
	DB::ReadingDBTool *srcDB;
	DB::DBReader *r;
	DB::TableDef *tabDef;
	Data::ArrayList<const UTF8Char *> tables;
	UOSInt i;
	UOSInt j;
	OSInt k;
	destDB = DB::SQLiteFile::CreateDBTool(fileName, &log, (const UTF8Char*)"DB: ");
	if (destDB == 0)
		return false;
	Bool succ = true;
	DB::SQLBuilder sql(destDB);
	Text::StringBuilderUTF8 sb;
	sDB = (DB::ReadingDB *)pobj;
	sDB->GetTableNames(&tables);
	if (sDB->IsFullConn())
	{
		NEW_CLASS(srcDB, DB::ReadingDBTool((DB::DBConn*)sDB, false, &log, (const UTF8Char*)"SDB: "));
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
			tabDef = srcDB->GetTableDef(tables.GetItem(i));
			if (tabDef)
			{
				r = srcDB->GetTableData(tables.GetItem(i), 0, 0, 0, 0, 0);
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
			r = sDB->GetTableData(tables.GetItem(i), 0, 0, 0, 0, 0);
			if (r)
			{
				tabDef = r->GenTableDef(tables.GetItem(i));
			}
			else
			{
				tabDef = 0;
			}
		}
		if (r)
		{
			sql.Clear();
			destDB->GenCreateTableCmd(&sql, tables.GetItem(i), tabDef);
			if (destDB->ExecuteNonQueryC(sql.ToString(), sql.GetLength()) <= -2)
			{
				IO::FileStream *debugFS;
				Text::UTF8Writer *debugWriter;
				NEW_CLASS(debugFS, IO::FileStream(CSTR("Debug.txt"), IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
				NEW_CLASS(debugWriter, Text::UTF8Writer(debugFS));
				debugWriter->WriteLineC(sql.ToString(), sql.GetLength());
				DEL_CLASS(debugWriter);
				DEL_CLASS(debugFS);

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
				destDB->GenInsertCmd(&sql, tables.GetItem(i), r);
				if (destDB->ExecuteNonQueryC(sql.ToString(), sql.GetLength()) <= 0)
				{
					sb.ClearStr();
					destDB->GetLastErrorMsg(&sb);

					IO::FileStream *debugFS;
					Text::UTF8Writer *debugWriter;
					NEW_CLASS(debugFS, IO::FileStream(CSTR("Debug.txt"), IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
					NEW_CLASS(debugWriter, Text::UTF8Writer(debugFS));
					debugWriter->WriteLineC(sql.ToString(), sql.GetLength());
					debugWriter->WriteLineC(sb.ToString(), sb.GetLength());
					DEL_CLASS(debugWriter);
					DEL_CLASS(debugFS);
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

	SDEL_CLASS(srcDB);
	DEL_CLASS(destDB);
	return succ;
}
