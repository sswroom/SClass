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

IO::FileExporter::SupportType Exporter::DBFExporter::IsObjectSupported(NN<IO::ParsedObject> pobj)
{
	if (pobj->GetParserType() != IO::ParserType::ReadingDB && pobj->GetParserType() != IO::ParserType::MapLayer)
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	NN<DB::ReadingDB> conn = NN<DB::ReadingDB>::ConvertFrom(pobj);
	UOSInt tableCnt;
	Data::ArrayListStringNN tableNames;
	conn->QueryTableNames(nullptr, tableNames);
	tableCnt = tableNames.GetCount();
	tableNames.FreeAll();
	if (tableCnt == 1)
		return IO::FileExporter::SupportType::NormalStream;
	return IO::FileExporter::SupportType::NotSupported;
}

Bool Exporter::DBFExporter::GetOutputName(UOSInt index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff)
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

Bool Exporter::DBFExporter::ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param)
{
	if (pobj->GetParserType() != IO::ParserType::ReadingDB && pobj->GetParserType() != IO::ParserType::MapLayer)
	{
		return false;
	}
	UTF8Char sbuff[1024];
	UnsafeArray<UTF8Char> sptr;
	NN<DB::ReadingDB> conn = NN<DB::ReadingDB>::ConvertFrom(pobj);
	UOSInt tableCnt;
	Data::ArrayListStringNN tableNames;
	conn->QueryTableNames(nullptr, tableNames);
	tableCnt = tableNames.GetCount();
	NN<Text::String> tableName;
	if (tableCnt != 1 || !tableNames.GetItem(0).SetTo(tableName))
	{
		tableNames.FreeAll();
		return false;
	}

	NN<DB::DBReader> r;
	if (!conn->QueryTableData(nullptr, tableName->ToCString(), 0, 0, 0, nullptr, 0).SetTo(r))
	{
		tableName->Release();
		return false;
	}
	tableName->Release();
	UOSInt nCol;
	UOSInt nOut;

	Text::String **colNames;
	UOSInt *colSize;
	UOSInt *colDP;
	DB::DBUtil::ColType *colTypes;
	UOSInt *colMap;
	UOSInt i;
	UOSInt j;
	DB::DBFFixWriter *writer;
	Data::DateTime dt;

	nCol = r->ColCount();
	colNames = MemAlloc(Text::String *, nCol);
	colSize = MemAlloc(UOSInt, nCol);
	colDP = MemAlloc(UOSInt, nCol);
	colTypes = MemAlloc(DB::DBUtil::ColType, nCol);
	colMap = MemAlloc(UOSInt, nCol);

	{
		DB::ColDef colDef(Text::String::NewEmpty());
		i = 0;
		nOut = 0;
		while (i < nCol)
		{
			r->GetColDef(i, colDef);
			if (colDef.GetColType() == DB::DBUtil::CT_Vector)
			{
			}
			else
			{
				colMap[nOut] = i;
				colNames[nOut] = colDef.GetColName()->Clone().Ptr();
				if (colDef.GetColType() == DB::DBUtil::CT_DateTime)
				{
					colSize[nOut] = 8;
					colDP[nOut] = 0;
					colTypes[nOut] = DB::DBUtil::CT_DateTime;
				}
				else
				{
					colSize[nOut] = colDef.GetColSize();
					colDP[nOut] = colDef.GetColDP();
					colTypes[nOut] = colDef.GetColType();
					if (colSize[nOut] > 255)
					{
						colSize[nOut] = 255;
					}
				}
				nOut++;
			}
			i++;
		}
	}

	NEW_CLASS(writer, DB::DBFFixWriter(stm, nOut, colNames, colSize, colDP, colTypes, this->codePage));
	
	while (r->ReadNext())
	{
		i = nOut;
		while (i-- > 0)
		{
			j = colMap[i];
			if (colTypes[i] == DB::DBUtil::CT_UTF8Char || colTypes[i] == DB::DBUtil::CT_VarUTF8Char)
			{
				sptr = r->GetStr(j, sbuff, sizeof(sbuff)).Or(sbuff);
				writer->SetColumnStr(i, CSTRP(sbuff, sptr));
			}
			else if (colTypes[i] == DB::DBUtil::CT_Int16)
			{
				writer->SetColumnI16(i, (Int16)r->GetInt32(j));
			}
			else if (colTypes[i] == DB::DBUtil::CT_Int32)
			{
				writer->SetColumnI32(i, r->GetInt32(j));
			}
			else if (colTypes[i] == DB::DBUtil::CT_Int64)
			{
				writer->SetColumnI64(i, r->GetInt64(j));
			}
			else if (colTypes[i] == DB::DBUtil::CT_Double)
			{
				writer->SetColumnF64(i, r->GetDblOrNAN(j));
			}
			else if (colTypes[i] == DB::DBUtil::CT_DateTime)
			{
				writer->SetColumnTS(i, r->GetTimestamp(j));
			}
			else
			{
				sptr = r->GetStr(j, sbuff, sizeof(sbuff)).Or(sbuff);
				writer->SetColumnStr(i, CSTRP(sbuff, sptr));
			}
		}
		writer->WriteRecord();
	}
	////////////////////////////////////////

	DEL_CLASS(writer);
	conn->CloseReader(r);

	i = nOut;
	while (i-- > 0)
	{
		colNames[i]->Release();
	}
	MemFree(colMap);
	MemFree(colNames);
	MemFree(colSize);
	MemFree(colDP);
	MemFree(colTypes);
	return true;
}
