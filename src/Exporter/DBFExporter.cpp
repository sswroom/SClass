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
	if (pobj->GetParserType() != IO::ParserType::ReadingDB && pobj->GetParserType() != IO::ParserType::MapLayer)
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	DB::ReadingDB *conn = (DB::ReadingDB *)pobj;
	UOSInt tableCnt;
	Data::ArrayListNN<Text::String> tableNames;
	conn->QueryTableNames(CSTR_NULL, tableNames);
	tableCnt = tableNames.GetCount();
	LIST_FREE_STRING(&tableNames);
	if (tableCnt == 1)
		return IO::FileExporter::SupportType::NormalStream;
	return IO::FileExporter::SupportType::NotSupported;
}

Bool Exporter::DBFExporter::GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcatC(nameBuff, UTF8STRC("DBase File"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.dbf"));
		return true;
	}
	return false;
}

void Exporter::DBFExporter::SetCodePage(UInt32 codePage)
{
	this->codePage = codePage;
}

Bool Exporter::DBFExporter::ExportFile(NotNullPtr<IO::SeekableStream> stm, Text::CStringNN fileName, IO::ParsedObject *pobj, void *param)
{
	if (pobj->GetParserType() != IO::ParserType::ReadingDB && pobj->GetParserType() != IO::ParserType::MapLayer)
	{
		return false;
	}
	UTF8Char sbuff[1024];
	DB::ReadingDB *conn = (DB::ReadingDB *)pobj;
	UOSInt tableCnt;
	Data::ArrayListNN<Text::String> tableNames;
	conn->QueryTableNames(CSTR_NULL, tableNames);
	tableCnt = tableNames.GetCount();
	if (tableCnt != 1)
	{
		LIST_FREE_STRING(&tableNames);
		return false;
	}
	Text::String *tableName = tableNames.GetItem(0);

	NotNullPtr<DB::DBReader> r;
	if (!r.Set(conn->QueryTableData(CSTR_NULL, tableName->ToCString(), 0, 0, 0, CSTR_NULL, 0)))
	{
		tableName->Release();
		return false;
	}
	tableName->Release();
	UOSInt nCol;

	Text::String **colNames;
	UOSInt *colSize;
	UOSInt *colDP;
	DB::DBUtil::ColType *colTypes;
	UOSInt i;
	DB::DBFFixWriter *writer;
	Data::DateTime dt;

	nCol = r->ColCount();
	colNames = MemAlloc(Text::String *, nCol);
	colSize = MemAlloc(UOSInt, nCol);
	colDP = MemAlloc(UOSInt, nCol);
	colTypes = MemAlloc(DB::DBUtil::ColType, nCol);

	{
		DB::ColDef colDef(Text::String::NewEmpty());
		i = nCol;
		while (i-- > 0)
		{
			r->GetColDef(i, colDef);
			colNames[i] = colDef.GetColName()->Clone().Ptr();
			if (colDef.GetColType() == DB::DBUtil::CT_DateTime)
			{
				colSize[i] = 8;
				colDP[i] = 0;
				colTypes[i] = DB::DBUtil::CT_DateTime;
			}
			else
			{
				colSize[i] = colDef.GetColSize();
				colDP[i] = colDef.GetColDP();
				colTypes[i] = colDef.GetColType();
				if (colSize[i] > 255)
				{
					colSize[i] = 255;
				}
			}
		}
	}

	NEW_CLASS(writer, DB::DBFFixWriter(stm, nCol, colNames, colSize, colDP, colTypes, this->codePage));
	
	while (r->ReadNext())
	{
		i = nCol;
		while (i-- > 0)
		{
			if (colTypes[i] == DB::DBUtil::CT_UTF8Char || colTypes[i] == DB::DBUtil::CT_VarUTF8Char)
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
				writer->SetColumn(i, r->GetTimestamp(i));
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
		colNames[i]->Release();
	}
	MemFree(colNames);
	MemFree(colSize);
	MemFree(colDP);
	MemFree(colTypes);
	return true;
}
