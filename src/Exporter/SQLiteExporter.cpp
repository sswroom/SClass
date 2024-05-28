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

IO::FileExporter::SupportType Exporter::SQLiteExporter::IsObjectSupported(NN<IO::ParsedObject> pobj)
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

Bool Exporter::SQLiteExporter::ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param)
{
	if (pobj->GetParserType() != IO::ParserType::ReadingDB)
	{
		return false;
	}
	IO::Path::DeleteFile(fileName.v);
	NN<DB::DBTool> destDB;
	IO::LogTool log;
	NN<DB::ReadingDB> sDB;
	DB::ReadingDBTool *srcDB;
	Optional<DB::DBReader> r;
	NN<DB::DBReader> nnr;
	Optional<DB::TableDef> tabDef;
	NN<DB::TableDef> nntabDef;
	Data::ArrayListStringNN tables;
	OSInt k;
	if (!DB::SQLiteFile::CreateDBTool(fileName.OrEmpty(), log, CSTR("DB: ")).SetTo(destDB))
		return false;
	Bool succ = true;
	DB::SQLBuilder sql(destDB);
	Text::StringBuilderUTF8 sb;
	sDB = NN<DB::ReadingDB>::ConvertFrom(pobj);
	sDB->QueryTableNames(CSTR_NULL, tables);
	if (sDB->IsFullConn())
	{
		NEW_CLASS(srcDB, DB::ReadingDBTool(NN<DB::DBConn>::ConvertFrom(sDB), false, log, CSTR("SDB: ")));
	}
	else
	{
		srcDB = 0;
	}
	Data::ArrayIterator<NN<Text::String>> it = tables.Iterator();
	NN<Text::String> tabName;
	while (it.HasNext())
	{
		tabName = it.Next();
		if (srcDB)
		{
			if (srcDB->GetTableDef(CSTR_NULL, tabName->ToCString()).SetTo(nntabDef))
			{
				r = srcDB->QueryTableData(CSTR_NULL, tabName->ToCString(), 0, 0, 0, CSTR_NULL, 0);
				if (r.IsNull())
				{
					nntabDef.Delete();
					tabDef = 0;
				}
				else
				{
					tabDef = nntabDef;
				}
			}
			else
			{
				tabDef = 0;
				r = 0;
			}
		}
		else
		{
			r = sDB->QueryTableData(CSTR_NULL, tabName->ToCString(), 0, 0, 0, CSTR_NULL, 0);
			if (r.SetTo(nnr))
			{
				tabDef = nnr->GenTableDef(CSTR_NULL, tabName->ToCString());
			}
			else
			{
				tabDef = 0;
			}
		}
		if (r.SetTo(nnr))
		{
			if (!tabDef.SetTo(nntabDef))
			{
				{
					IO::FileStream debugFS(CSTR("Debug.txt"), IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
					Text::UTF8Writer debugWriter(debugFS);
					debugWriter.WriteLine(sql.ToCString());
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
					debugWriter.WriteLine(sql.ToCString());
				}

				tabDef.Delete();
				succ = false;
				destDB->EndTrans(true);
				sDB->CloseReader(nnr);
				break;
			}
			tabDef.Delete();
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
					debugWriter.WriteLine(sql.ToCString());
					debugWriter.WriteLine(sb.ToCString());
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
