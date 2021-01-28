#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/ColDef.h"
#include "DB/DBTool.h"
#include "DB/MDBFile.h"
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
	return IO::FileExporter::ST_NOT_SUPPORTED;
#else
	if (pobj->GetParserType() != IO::ParsedObject::PT_READINGDB_PARSER && pobj->GetParserType() != IO::ParsedObject::PT_MAP_LAYER_PARSER)
	{
		return IO::FileExporter::ST_NOT_SUPPORTED;
	}
	return IO::FileExporter::ST_PATH_ONLY;
#endif
}

Bool Exporter::MDBExporter::GetOutputName(OSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcat(nameBuff, (const UTF8Char*)"Access File");
		Text::StrConcat(fileNameBuff, (const UTF8Char*)"*.mdb");
		return true;
	}
	return false;
}

Bool Exporter::MDBExporter::ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param)
{
#if defined(_WIN32_WCE)
	return false;
#else
	if (pobj->GetParserType() != IO::ParsedObject::PT_READINGDB_PARSER)
	{
		return false;
	}
	IO::Path::DeleteFile(fileName);
	if (!DB::MDBFile::CreateMDBFile(fileName))
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
	OSInt i;
	OSInt j;
	OSInt k;
	OSInt l;
	mdb = DB::MDBFile::CreateDBTool(fileName, &log, false, (const UTF8Char*)"DB: ");
	if (mdb == 0)
		return false;
	Bool succ = true;
	DB::SQLBuilder sql(mdb->GetSvrType());
	Text::StringBuilderUTF8 sb;
	srcDB = (DB::ReadingDB *)pobj;
	srcDB->GetTableNames(&tables);
	i = 0;
	j = tables.GetCount();
	while (i < j)
	{
		r = srcDB->GetTableData(tables.GetItem(i), 0, 0, 0);
		if (r)
		{
			NEW_CLASS(tabDef, DB::TableDef(tables.GetItem(i)));
			k = 0;
			l = r->ColCount();
			while (k < l)
			{
				NEW_CLASS(colDef, DB::ColDef((const UTF8Char*)""));
				r->GetColDef(k, colDef);
				tabDef->AddCol(colDef);
				k++;
			}
			sql.Clear();
			mdb->GenCreateTableCmd(&sql, tables.GetItem(i), tabDef);
			if (mdb->ExecuteNonQuery(sql.ToString()) <= -2)
			{
/*				IO::FileStream *debugFS;
				Text::UTF8Writer *debugWriter;
				NEW_CLASS(debugFS, IO::FileStream((const UTF8Char*)"I:\\ProgTest\\Debug.txt", IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
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
				if (mdb->ExecuteNonQuery(sql.ToString()) <= 0)
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

