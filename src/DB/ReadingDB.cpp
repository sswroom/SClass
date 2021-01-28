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

IO::ParsedObject::ParserType DB::ReadingDB::GetParserType()
{
	return IO::ParsedObject::PT_READINGDB_PARSER;
}

Bool DB::ReadingDB::IsFullConn()
{
	return false;
}

Bool DB::ReadingDB::IsDBObj(IO::ParsedObject *pobj)
{
	IO::ParsedObject::ParserType pt = pobj->GetParserType();
	if (pt == IO::ParsedObject::PT_MAP_LAYER_PARSER || pt == IO::ParsedObject::PT_READINGDB_PARSER)
	{
		return true;
	}
	return false;
}
