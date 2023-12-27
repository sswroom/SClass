#include "Stdafx.h"
#include "Data/DateTime.h"
#include "DB/ReadingDB.h"
#include "Text/MyString.h"

DB::ReadingDB::ReadingDB(NotNullPtr<Text::String> sourceName) : IO::ParsedObject(sourceName)
{
}

DB::ReadingDB::ReadingDB(Text::CStringNN sourceName) : IO::ParsedObject(sourceName)
{
}

DB::ReadingDB::~ReadingDB()
{
}

UOSInt DB::ReadingDB::QuerySchemaNames(NotNullPtr<Data::ArrayListStringNN> names)
{
	return 0;
}

Int8 DB::ReadingDB::GetTzQhr() const
{
	return Data::DateTimeUtil::GetLocalTzQhr();
}

UOSInt DB::ReadingDB::GetDatabaseNames(NotNullPtr<Data::ArrayListStringNN> arr)
{
	return 0;
}

void DB::ReadingDB::ReleaseDatabaseNames(NotNullPtr<Data::ArrayListStringNN> arr)
{

}

Bool DB::ReadingDB::ChangeDatabase(Text::CString databaseName)
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

Bool DB::ReadingDB::IsDBObj(NotNullPtr<IO::ParsedObject> pobj)
{
	IO::ParserType pt = pobj->GetParserType();
	if (pt == IO::ParserType::MapLayer || pt == IO::ParserType::ReadingDB)
	{
		return true;
	}
	return false;
}
