#include "Stdafx.h"
#include "Map/FileGDB.h"

Map::FileGDB::FileGDB(const UTF8Char *filePath) : DB::ReadingDB(filePath)
{
}

Map::FileGDB::~FileGDB()
{
}

Bool Map::FileGDB::IsError()
{
	return true;
}

UOSInt Map::FileGDB::GetTableNames(Data::ArrayList<const UTF8Char*> *names)
{
	return 0;
}

DB::DBReader *Map::FileGDB::GetTableData(const UTF8Char *name, UOSInt maxCnt, void *ordering, void *condition)
{
	return 0;
}

void Map::FileGDB::CloseReader(DB::DBReader *r)
{
}

void Map::FileGDB::GetErrorMsg(Text::StringBuilderUTF *str)
{
}

void Map::FileGDB::Reconnect()
{
}

