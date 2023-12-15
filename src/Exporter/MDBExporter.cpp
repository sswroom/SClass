#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/ColDef.h"
#include "DB/DBTool.h"
#include "DB/MDBFileConn.h"
#include "DB/ReadingDB.h"
#include "DB/SQLGenerator.h"
#include "Exporter/MDBExporter.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Writer.h"

Exporter::MDBExporter::MDBExporter()
{
}

Exporter::MDBExporter::~MDBExporter()
{
}

Int32 Exporter::MDBExporter::GetName()
{
	return *(Int32*)"MDBE";
}

IO::FileExporter::SupportType Exporter::MDBExporter::IsObjectSupported(NotNullPtr<IO::ParsedObject> pobj)
{
#if defined(_WIN32_WCE)
	return IO::FileExporter::SupportType::NotSupported;
#else
	if (pobj->GetParserType() != IO::ParserType::ReadingDB && pobj->GetParserType() != IO::ParserType::MapLayer)
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	return IO::FileExporter::SupportType::PathOnly;
#endif
}

Bool Exporter::MDBExporter::GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcatC(nameBuff, UTF8STRC("Access File"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.mdb"));
		return true;
	}
	return false;
}

Bool Exporter::MDBExporter::ExportFile(NotNullPtr<IO::SeekableStream> stm, Text::CStringNN fileName, NotNullPtr<IO::ParsedObject> pobj, void *param)
{
#if defined(_WIN32_WCE)
	return false;
#else
	if (pobj->GetParserType() != IO::ParserType::ReadingDB)
	{
		return false;
	}
	IO::Path::DeleteFile(fileName.v);
	if (!DB::MDBFileConn::CreateMDBFile(fileName))
	{
		return false;
	}
	NotNullPtr<DB::DBTool> mdb;
	IO::LogTool log;
	NotNullPtr<DB::ReadingDB> srcDB;
	NotNullPtr<DB::DBReader> r;
	NotNullPtr<DB::ColDef> colDef;
	Data::ArrayListNN<Text::String> tables;
	UOSInt i;
	UOSInt k;
	UOSInt l;
	if (!mdb.Set(DB::MDBFileConn::CreateDBTool(fileName, log, CSTR("DB: "))))
		return false;
	Bool succ = true;
	DB::SQLBuilder sql(mdb);
	Text::StringBuilderUTF8 sb;
	srcDB = NotNullPtr<DB::ReadingDB>::ConvertFrom(pobj);
	srcDB->QueryTableNames(CSTR_NULL, tables);
	Data::ArrayIterator<NotNullPtr<Text::String>> it = tables.Iterator();
	NotNullPtr<Text::String> tabName;
	while (it.HasNext())
	{
		tabName = it.Next();
		if (r.Set(srcDB->QueryTableData(CSTR_NULL, tabName->ToCString(), 0, 0, 0, CSTR_NULL, 0)))
		{
			DB::TableDef tabDef(CSTR_NULL, tabName->ToCString());
			k = 0;
			l = r->ColCount();
			while (k < l)
			{
				NEW_CLASSNN(colDef, DB::ColDef(Text::String::NewEmpty()));
				r->GetColDef(k, colDef);
				tabDef.AddCol(colDef);
				k++;
			}
			sql.Clear();
			DB::SQLGenerator::GenCreateTableCmd(sql, CSTR_NULL, tabName->ToCString(), tabDef, false);
			if (mdb->ExecuteNonQuery(sql.ToCString()) <= -2)
			{
/*				IO::FileStream *debugFS;
				Text::UTF8Writer *debugWriter;
				NEW_CLASS(debugFS, IO::FileStream(CSTR("I:\\ProgTest\\Debug.txt"), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
				NEW_CLASS(debugWriter, Text::UTF8Writer(debugFS));
				debugWriter->WriteLineW(sql.ToString());
				DEL_CLASS(debugWriter);
				DEL_CLASS(debugFS);*/
				succ = false;
				break;
			}
			///////////////////////////////////
			while (r->ReadNext())
			{
				sql.Clear();
				DB::SQLGenerator::GenInsertCmd(sql, CSTR_NULL, tabName->ToCString(), r);
				if (mdb->ExecuteNonQuery(sql.ToCString()) <= 0)
				{
					sb.ClearStr();
					mdb->GetLastErrorMsg(sb);
					succ = false;
					break;
				}
			}
			srcDB->CloseReader(r);
		}
		i++;
	}
	LISTNN_FREE_STRING(&tables);
	mdb.Delete();
	return succ;
#endif
}

