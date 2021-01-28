#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "DB/ColDef.h"
#include "DB/DBFFixWriter.h"
#include "DB/DBReader.h"
#include "DB/ReadingDB.h"
#include "Exporter/DBFExporter.h"

Exporter::DBFExporter::DBFExporter()
{
	this->codePage = 0;
}

Exporter::DBFExporter::~DBFExporter()
{
}

Int32 Exporter::DBFExporter::GetName()
{
	return *(Int32*)"DBFE";
}

IO::FileExporter::SupportType Exporter::DBFExporter::IsObjectSupported(IO::ParsedObject *pobj)
{
	if (pobj->GetParserType() != IO::ParsedObject::PT_READINGDB_PARSER && pobj->GetParserType() != IO::ParsedObject::PT_MAP_LAYER_PARSER)
	{
		return IO::FileExporter::ST_NOT_SUPPORTED;
	}
	DB::ReadingDB *conn = (DB::ReadingDB *)pobj;
	OSInt tableCnt;
	Data::ArrayList<const UTF8Char *> *tableNames;
	NEW_CLASS(tableNames, Data::ArrayList<const UTF8Char*>());
	conn->GetTableNames(tableNames);
	tableCnt = tableNames->GetCount();
	DEL_CLASS(tableNames);
	if (tableCnt == 1)
		return IO::FileExporter::ST_NORMAL_STREAM;
	return IO::FileExporter::ST_NOT_SUPPORTED;
}

Bool Exporter::DBFExporter::GetOutputName(OSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcat(nameBuff, (const UTF8Char*)"DBase File");
		Text::StrConcat(fileNameBuff, (const UTF8Char*)"*.dbf");
		return true;
	}
	return false;
}

void Exporter::DBFExporter::SetCodePage(Int32 codePage)
{
	this->codePage = codePage;
}

Bool Exporter::DBFExporter::ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param)
{
	if (pobj->GetParserType() != IO::ParsedObject::PT_READINGDB_PARSER && pobj->GetParserType() != IO::ParsedObject::PT_MAP_LAYER_PARSER)
	{
		return false;
	}
	UTF8Char sbuff[1024];
	DB::ReadingDB *conn = (DB::ReadingDB *)pobj;
	OSInt tableCnt;
	Data::ArrayList<const UTF8Char *> *tableNames;
	NEW_CLASS(tableNames, Data::ArrayList<const UTF8Char*>());
	conn->GetTableNames(tableNames);
	tableCnt = tableNames->GetCount();
	if (tableCnt != 1)
	{
		DEL_CLASS(tableNames);
		return false;
	}
	const UTF8Char *tableName = tableNames->GetItem(0);
	DEL_CLASS(tableNames);

	DB::DBReader *r = conn->GetTableData(tableName, 0, 0, 0);
	if (r == 0)
		return false;
	OSInt nCol;

	const UTF8Char **colNames;
	UOSInt *colSize;
	UOSInt *colDP;
	DB::DBUtil::ColType *colTypes;
	DB::ColDef *colDef;
	OSInt i;
	DB::DBFFixWriter *writer;
	Data::DateTime dt;

	nCol = r->ColCount();
	colNames = MemAlloc(const UTF8Char *, nCol);
	colSize = MemAlloc(UOSInt, nCol);
	colDP = MemAlloc(UOSInt, nCol);
	colTypes = MemAlloc(DB::DBUtil::ColType, nCol);

	NEW_CLASS(colDef, DB::ColDef((const UTF8Char*)""));
	i = nCol;
	while (i-- > 0)
	{
		r->GetColDef(i, colDef);
		colNames[i] = Text::StrCopyNew(colDef->GetColName());
		if (colDef->GetColType() == DB::DBUtil::CT_DateTime)
		{
			colSize[i] = 8;
			colDP[i] = 0;
			colTypes[i] = DB::DBUtil::CT_DateTime;
		}
		else
		{
			colSize[i] = colDef->GetColSize();
			colDP[i] = colDef->GetColDP();
			colTypes[i] = colDef->GetColType();
			if (colSize[i] > 255)
			{
				colSize[i] = 255;
			}
		}
	}
	DEL_CLASS(colDef);

	NEW_CLASS(writer, DB::DBFFixWriter(stm, nCol, colNames, colSize, colDP, colTypes, this->codePage));
	
	while (r->ReadNext())
	{
		i = nCol;
		while (i-- > 0)
		{
			if (colTypes[i] == DB::DBUtil::CT_Char || colTypes[i] == DB::DBUtil::CT_VarChar)
			{
				r->GetStr(i, sbuff, sizeof(sbuff));
				writer->SetColumn(i, sbuff);
			}
			else if (colTypes[i] == DB::DBUtil::CT_Int16)
			{
				writer->SetColumn(i, (Int16)r->GetInt32(i));
			}
			else if (colTypes[i] == DB::DBUtil::CT_Int32)
			{
				writer->SetColumn(i, r->GetInt32(i));
			}
			else if (colTypes[i] == DB::DBUtil::CT_Int64)
			{
				writer->SetColumn(i, r->GetInt64(i));
			}
			else if (colTypes[i] == DB::DBUtil::CT_Double)
			{
				writer->SetColumn(i, r->GetDbl(i));
			}
			else if (colTypes[i] == DB::DBUtil::CT_DateTime)
			{
				r->GetDate(i, &dt);
				writer->SetColumn(i, &dt);
			}
			else
			{
				r->GetStr(i, sbuff, sizeof(sbuff));
				writer->SetColumn(i, sbuff);
			}
		}
		writer->WriteRecord();
	}
	////////////////////////////////////////

	DEL_CLASS(writer);
	conn->CloseReader(r);

	i = nCol;
	while (i-- > 0)
	{
		Text::StrDelNew(colNames[i]);
	}
	MemFree(colNames);
	MemFree(colSize);
	MemFree(colDP);
	MemFree(colTypes);
	return true;
}
