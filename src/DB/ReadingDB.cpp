#include "Stdafx.h"
#include "Data/DateTime.h"
#include "DB/ReadingDB.h"
#include "Text/MyString.h"

DB::ReadingDB::ReadingDB(NN<Text::String> sourceName) : IO::ParsedObject(sourceName)
{
}

DB::ReadingDB::ReadingDB(Text::CStringNN sourceName) : IO::ParsedObject(sourceName)
{
}

DB::ReadingDB::~ReadingDB()
{
}

UOSInt DB::ReadingDB::QuerySchemaNames(NN<Data::ArrayListStringNN> names)
{
	return 0;
}

Int8 DB::ReadingDB::GetTzQhr() const
{
	return Data::DateTimeUtil::GetLocalTzQhr();
}

UOSInt DB::ReadingDB::GetDatabaseNames(NN<Data::ArrayListStringNN> arr)
{
	return 0;
}

void DB::ReadingDB::ReleaseDatabaseNames(NN<Data::ArrayListStringNN> arr)
{

}

Bool DB::ReadingDB::ChangeDatabase(Text::CStringNN databaseName)
{
	return false;
}

Text::String *DB::ReadingDB::GetCurrDBName()
{
	return 0;
}

IO::ParserType DB::ReadingDB::GetParserType() const
{
	return IO::ParserType::ReadingDB;
}

Bool DB::ReadingDB::IsFullConn() const
{
	return false;
}

Bool DB::ReadingDB::IsDBTool() const
{
	return false;
}

Bool DB::ReadingDB::IsDBObj(NN<IO::ParsedObject> pobj)
{
	IO::ParserType pt = pobj->GetParserType();
	if (pt == IO::ParserType::MapLayer || pt == IO::ParserType::ReadingDB)
	{
		return true;
	}
	return false;
}
