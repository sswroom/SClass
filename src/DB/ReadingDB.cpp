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

UOSInt DB::ReadingDB::QuerySchemaNames(Data::ArrayList<Text::String*> *names)
{
	return 0;
}

UOSInt DB::ReadingDB::GetDatabaseNames(Data::ArrayListNN<Text::String> *arr)
{
	return 0;
}

void DB::ReadingDB::ReleaseDatabaseNames(Data::ArrayListNN<Text::String> *arr)
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

Bool DB::ReadingDB::IsFullConn()
{
	return false;
}

Bool DB::ReadingDB::IsDBTool()
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
