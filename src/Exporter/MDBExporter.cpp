#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/ColDef.h"
#include "DB/DBTool.h"
#include "DB/MDBFileConn.h"
#include "DB/ReadingDB.h"
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

IO::FileExporter::SupportType Exporter::MDBExporter::IsObjectSupported(IO::ParsedObject *pobj)
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

Bool Exporter::MDBExporter::ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param)
{
#if defined(_WIN32_WCE)
	return false;
#else
	if (pobj->GetParserType() != IO::ParserType::ReadingDB)
	{
		return false;
	}
	UOSInt fileNameLen = Text::StrCharCnt(fileName);
	IO::Path::DeleteFile(fileName);
	if (!DB::MDBFileConn::CreateMDBFile({fileName, fileNameLen}))
	{
		return false;
	}
	DB::DBTool *mdb;
	IO::LogTool log;
	DB::ReadingDB *srcDB;
	DB::DBReader *r;
	DB::TableDef *tabDef;
	DB::ColDef *colDef;
	Data::ArrayList<const UTF8Char *> tables;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	mdb = DB::MDBFileConn::CreateDBTool({fileName, fileNameLen}, &log, CSTR("DB: "));
	if (mdb == 0)
		return false;
	Bool succ = true;
	DB::SQLBuilder sql(mdb);
	Text::StringBuilderUTF8 sb;
	srcDB = (DB::ReadingDB *)pobj;
	srcDB->GetTableNames(&tables);
	i = 0;
	j = tables.GetCount();
	while (i < j)
	{
		r = srcDB->GetTableData(tables.GetItem(i), 0, 0, 0, CSTR_NULL, 0);
		if (r)
		{
			NEW_CLASS(tabDef, DB::TableDef(tables.GetItem(i)));
			k = 0;
			l = r->ColCount();
			while (k < l)
			{
				NEW_CLASS(colDef, DB::ColDef(CSTR("")));
				r->GetColDef(k, colDef);
				tabDef->AddCol(colDef);
				k++;
			}
			sql.Clear();
			mdb->GenCreateTableCmd(&sql, tables.GetItem(i), tabDef);
			if (mdb->ExecuteNonQueryC(sql.ToString(), sql.GetLength()) <= -2)
			{
/*				IO::FileStream *debugFS;
				Text::UTF8Writer *debugWriter;
				NEW_CLASS(debugFS, IO::FileStream(CSTR("I:\\ProgTest\\Debug.txt"), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
				NEW_CLASS(debugWriter, Text::UTF8Writer(debugFS));
				debugWriter->WriteLineW(sql.ToString());
				DEL_CLASS(debugWriter);
				DEL_CLASS(debugFS);*/
				DEL_CLASS(tabDef);
				succ = false;
				break;
			}
			///////////////////////////////////
			DEL_CLASS(tabDef);
			while (r->ReadNext())
			{
				sql.Clear();
				mdb->GenInsertCmd(&sql, tables.GetItem(i), r);
				if (mdb->ExecuteNonQueryC(sql.ToString(), sql.GetLength()) <= 0)
				{
					sb.ClearStr();
					mdb->GetLastErrorMsg(&sb);
					succ = false;
					break;
				}
			}
			srcDB->CloseReader(r);
		}
		i++;
	}

	DEL_CLASS(mdb);
	return succ;
#endif
}

