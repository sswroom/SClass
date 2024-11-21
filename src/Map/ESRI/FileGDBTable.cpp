#include "Stdafx.h"
#include "Data/ByteBuffer.h"
#include "Data/ByteTool.h"
#include "Map/ESRI/FileGDBReader.h"
#include "Map/ESRI/FileGDBTable.h"

Map::ESRI::FileGDBTable::FileGDBTable(Text::CStringNN tableName, NN<IO::StreamData> gdbtableFD, Optional<IO::StreamData> gdbtablxFD, NN<Math::ArcGISPRJParser> prjParser)
{
	this->tableName = Text::String::New(tableName);
	this->gdbtableFD = gdbtableFD->GetPartialData(0, gdbtableFD->GetDataSize());
	this->gdbtablxFD = 0;
	this->indexCnt = 0;
	this->tableInfo = 0;
	this->dataOfst = 0;
	this->maxRowSize = 0;
	this->prjParser = prjParser;

	NN<IO::StreamData> nngdbtablxFD;
	UInt8 hdrBuff[44];
	if (gdbtablxFD.SetTo(nngdbtablxFD) && nngdbtablxFD->GetRealData(0, 16, BYTEARR(hdrBuff)) == 16)
	{
		if (ReadUInt32(&hdrBuff[0]) == 3 && ReadUInt32(&hdrBuff[12]) == 5 && (ReadUInt32(&hdrBuff[4]) >= 1 && ReadUInt32(&hdrBuff[4]) <= 10))
		{
			this->indexCnt = ReadUInt32(&hdrBuff[8]);
			if (nngdbtablxFD->GetDataSize() >= 16 + this->indexCnt * 5)
			{
				this->gdbtablxFD = nngdbtablxFD->GetPartialData(0, nngdbtablxFD->GetDataSize());
			}
		}
	}
	if (this->gdbtableFD->GetRealData(0, 44, BYTEARR(hdrBuff)) != 44)
	{
		return;
	}
	if (ReadUInt32(&hdrBuff[0]) != 3 || ReadUInt32(&hdrBuff[12]) != 5 || ReadUInt32(&hdrBuff[20]) != 0 || ReadUInt64(&hdrBuff[24]) != this->gdbtableFD->GetDataSize())
	{
		return;
	}
	this->maxRowSize = ReadUInt32(&hdrBuff[8]);
	UInt64 fieldDescOfst = ReadUInt64(&hdrBuff[32]);
	UInt64 fileLength = this->gdbtableFD->GetDataSize();
	if (fieldDescOfst == 40)
	{
		UInt32 fieldSize = ReadUInt32(&hdrBuff[40]);
		Data::ByteBuffer fieldDesc(fieldSize + 4);
		this->gdbtableFD->GetRealData(40, fieldSize + 4, fieldDesc);
		this->tableInfo = Map::ESRI::FileGDBUtil::ParseFieldDesc(fieldDesc, this->prjParser);
		this->dataOfst = 40 + 4 + fieldSize;
	}
	else if (fieldDescOfst >= 40 && fieldDescOfst + 4 <= fileLength)
	{
		this->gdbtableFD->GetRealData(fieldDescOfst, 4, BYTEARR(hdrBuff).SubArray(40));
		UInt32 fieldSize = ReadUInt32(&hdrBuff[40]);
		if (fieldDescOfst + 4 + fieldSize <= fileLength)
		{
			Data::ByteBuffer fieldDesc(fieldSize + 4);
			this->gdbtableFD->GetRealData(fieldDescOfst, fieldSize + 4, fieldDesc);
			this->tableInfo = Map::ESRI::FileGDBUtil::ParseFieldDesc(fieldDesc, this->prjParser);
			this->dataOfst = fieldDescOfst + 4 + fieldSize;
		}
	}
}

Map::ESRI::FileGDBTable::~FileGDBTable()
{
	this->gdbtableFD.Delete();
	this->gdbtablxFD.Delete();
	this->tableName->Release();
	NN<FileGDBTableInfo> tableInfo;
	if (this->tableInfo.SetTo(tableInfo))
	{
		Map::ESRI::FileGDBUtil::FreeTableInfo(tableInfo);
		this->tableInfo = 0;
	}
}

Bool Map::ESRI::FileGDBTable::IsError()
{
	return this->tableInfo.IsNull();
}

NN<Text::String> Map::ESRI::FileGDBTable::GetName() const
{
	return this->tableName;
}

NN<Text::String> Map::ESRI::FileGDBTable::GetFileName() const
{
	return this->gdbtableFD->GetFullFileName();
}

Optional<DB::DBReader> Map::ESRI::FileGDBTable::OpenReader(Optional<Data::ArrayListStringNN> columnNames, UOSInt dataOfst, UOSInt maxCnt, Text::CString ordering, Optional<Data::QueryConditions> conditions)
{
	NN<FileGDBTableInfo> tableInfo;
	if (!this->tableInfo.SetTo(tableInfo))
	{
		return 0;
	}
	NN<Map::ESRI::FileGDBReader> reader;
	NEW_CLASSNN(reader, Map::ESRI::FileGDBReader(this->gdbtableFD, this->dataOfst, tableInfo, columnNames, dataOfst, maxCnt, conditions, this->maxRowSize));
	NN<IO::StreamData> fd;
	if (this->gdbtablxFD.SetTo(fd))
	{
		reader->SetIndex(fd, this->indexCnt);
	}
	return reader;
}
