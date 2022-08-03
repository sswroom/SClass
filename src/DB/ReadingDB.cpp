#include "Stdafx.h"
#include "Data/DateTime.h"
#include "DB/ReadingDB.h"
#include "Text/MyString.h"

DB::ReadingDB::ReadingDB(Text::String *sourceName) : IO::ParsedObject(sourceName)
{
}

DB::ReadingDB::ReadingDB(Text::CString sourceName) : IO::ParsedObject(sourceName)
{
}

DB::ReadingDB::~ReadingDB()
{
}

UOSInt DB::ReadingDB::QuerySchemaNames(Data::ArrayList<Text::String*> *names)
{
	return 0;
}

IO::ParserType DB::ReadingDB::GetParserType() const
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
