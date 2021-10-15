#include "Stdafx.h"
#include "Data/DateTime.h"
#include "DB/ReadingDB.h"
#include "Text/MyString.h"

DB::ReadingDB::ReadingDB(const UTF8Char *sourceName) : IO::ParsedObject(sourceName)
{
}

DB::ReadingDB::~ReadingDB()
{
}

IO::ParserType DB::ReadingDB::GetParserType()
{
	return IO::ParserType::ReadingDB;
}

Bool DB::ReadingDB::IsFullConn()
{
	return false;
}

Bool DB::ReadingDB::IsDBObj(IO::ParsedObject *pobj)
{
	IO::ParserType pt = pobj->GetParserType();
	if (pt == IO::ParserType::MapLayer || pt == IO::ParserType::ReadingDB)
	{
		return true;
	}
	return false;
}
