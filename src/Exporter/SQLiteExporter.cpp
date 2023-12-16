#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/ColDef.h"
#include "DB/DBTool.h"
#include "DB/SQLiteFile.h"
#include "DB/ReadingDB.h"
#include "DB/ReadingDBTool.h"
#include "DB/SQLGenerator.h"
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

IO::FileExporter::SupportType Exporter::SQLiteExporter::IsObjectSupported(NotNullPtr<IO::ParsedObject> pobj)
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

Bool Exporter::SQLiteExporter::ExportFile(NotNullPtr<IO::SeekableStream> stm, Text::CStringNN fileName, NotNullPtr<IO::ParsedObject> pobj, void *param)
{
	if (pobj->GetParserType() != IO::ParserType::ReadingDB)
	{
		return false;
	}
	IO::Path::DeleteFile(fileName.v);
	NotNullPtr<DB::DBTool> destDB;
	IO::LogTool log;
	NotNullPtr<DB::ReadingDB> sDB;
	DB::ReadingDBTool *srcDB;
	DB::DBReader *r;
	NotNullPtr<DB::DBReader> nnr;
	DB::TableDef *tabDef;
	NotNullPtr<DB::TableDef> nntabDef;
	Data::ArrayListStringNN tables;
	OSInt k;
	if (!DB::SQLiteFile::CreateDBTool(fileName.OrEmpty(), log, CSTR("DB: ")).SetTo(destDB))
		return false;
	Bool succ = true;
	DB::SQLBuilder sql(destDB);
	Text::StringBuilderUTF8 sb;
	sDB = NotNullPtr<DB::ReadingDB>::ConvertFrom(pobj);
	sDB->QueryTableNames(CSTR_NULL, tables);
	if (sDB->IsFullConn())
	{
		NEW_CLASS(srcDB, DB::ReadingDBTool(NotNullPtr<DB::DBConn>::ConvertFrom(sDB), false, log, CSTR("SDB: ")));
	}
	else
	{
		srcDB = 0;
	}
	Data::ArrayIterator<NotNullPtr<Text::String>> it = tables.Iterator();
	NotNullPtr<Text::String> tabName;
	while (it.HasNext())
	{
		tabName = it.Next();
		if (srcDB)
		{
			tabDef = srcDB->GetTableDef(CSTR_NULL, tabName->ToCString());
			if (tabDef)
			{
				r = srcDB->QueryTableData(CSTR_NULL, tabName->ToCString(), 0, 0, 0, CSTR_NULL, 0);
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
			r = sDB->QueryTableData(CSTR_NULL, tabName->ToCString(), 0, 0, 0, CSTR_NULL, 0);
			if (r)
			{
				tabDef = r->GenTableDef(CSTR_NULL, tabName->ToCString());
			}
			else
			{
				tabDef = 0;
			}
		}
		if (nnr.Set(r))
		{
			if (!nntabDef.Set(tabDef))
			{
				{
					IO::FileStream debugFS(CSTR("Debug.txt"), IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
					Text::UTF8Writer debugWriter(debugFS);
					debugWriter.WriteLineCStr(sql.ToCString());
				}
				succ = false;
				destDB->EndTrans(true);
				sDB->CloseReader(nnr);
				break;
			}
			sql.Clear();
			DB::SQLGenerator::GenCreateTableCmd(sql, CSTR_NULL, tabName->ToCString(), nntabDef, false);
			if (destDB->ExecuteNonQuery(sql.ToCString()) <= -2)
			{
				{
					IO::FileStream debugFS(CSTR("Debug.txt"), IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
					Text::UTF8Writer debugWriter(debugFS);
					debugWriter.WriteLineCStr(sql.ToCString());
				}

				DEL_CLASS(tabDef);
				succ = false;
				destDB->EndTrans(true);
				sDB->CloseReader(nnr);
				break;
			}
			DEL_CLASS(tabDef);
			destDB->BeginTrans();
			k = 10000;
			while (nnr->ReadNext())
			{
				if (k-- <= 0)
				{
					destDB->EndTrans(true);
					destDB->BeginTrans();
					k = 10000;
				}
				sql.Clear();
				DB::SQLGenerator::GenInsertCmd(sql, CSTR_NULL, tabName->ToCString(), nnr);
				if (destDB->ExecuteNonQuery(sql.ToCString()) <= 0)
				{
					sb.ClearStr();
					destDB->GetLastErrorMsg(sb);

					IO::FileStream debugFS(CSTR("Debug.txt"), IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
					Text::UTF8Writer debugWriter(debugFS);
					debugWriter.WriteLineCStr(sql.ToCString());
					debugWriter.WriteLineCStr(sb.ToCString());
					succ = false;
					break;
				}
			}
			destDB->EndTrans(true);
			sDB->CloseReader(nnr);
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
	}

	tables.FreeAll();
	SDEL_CLASS(srcDB);
	destDB.Delete();
	return succ;
}
