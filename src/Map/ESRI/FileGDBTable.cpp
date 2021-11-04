#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Map/ESRI/FileGDBReader.h"
#include "Map/ESRI/FileGDBTable.h"

Map::ESRI::FileGDBTable::FileGDBTable(const UTF8Char *tableName, IO::IStreamData *fd)
{
	this->tableName = Text::StrCopyNew(tableName);
	this->fd = fd->GetPartialData(0, fd->GetDataSize());
	this->tableInfo = 0;
	this->dataOfst = 0;

	UInt8 hdrBuff[44];
	if (this->fd->GetRealData(0, 44, hdrBuff) != 44)
	{
		return;
	}
	if (ReadUInt32(&hdrBuff[0]) != 3 || ReadUInt32(&hdrBuff[12]) != 5 || ReadUInt32(&hdrBuff[20]) != 0 || ReadUInt64(&hdrBuff[24]) != this->fd->GetDataSize())
	{
		return;
	}
	if (ReadUInt64(&hdrBuff[32]) == 40)
	{
		UInt32 fieldSize = ReadUInt32(&hdrBuff[40]);
		UInt8 *fieldDesc = MemAlloc(UInt8, fieldSize + 4);
		this->fd->GetRealData(40, fieldSize + 4, fieldDesc);
		this->tableInfo = Map::ESRI::FileGDBUtil::ParseFieldDesc(fieldDesc);
		MemFree(fieldDesc);
		this->dataOfst = 40 + 4 + fieldSize;
	}
}

Map::ESRI::FileGDBTable::~FileGDBTable()
{
	DEL_CLASS(this->fd);
	Text::StrDelNew(this->tableName);
	if (this->tableInfo)
	{
		Map::ESRI::FileGDBUtil::FreeTableInfo(this->tableInfo);
		this->tableInfo = 0;
	}
}

Bool Map::ESRI::FileGDBTable::IsError()
{
	return this->tableInfo == 0;
}

const UTF8Char *Map::ESRI::FileGDBTable::GetName()
{
	return this->tableName;
}

DB::DBReader *Map::ESRI::FileGDBTable::OpenReader()
{
	if (this->tableInfo == 0)
	{
		return 0;
	}
	Map::ESRI::FileGDBReader *reader;
	NEW_CLASS(reader, Map::ESRI::FileGDBReader(this->fd, this->dataOfst, this->tableInfo));
	return reader;
}
