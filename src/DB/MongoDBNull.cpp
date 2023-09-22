#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/MongoDB.h"

Int32 DB::MongoDB::initCnt = 0;

DB::MongoDB::MongoDB(Text::CStringNN url, Text::CString database, IO::LogTool *log) : DB::ReadingDB(url)
{
}

DB::MongoDB::~MongoDB()
{
}

UOSInt DB::MongoDB::QueryTableNames(Text::CString schemaName, NotNullPtr<Data::ArrayListNN<Text::String>> names)
{
	return 0;
}

DB::DBReader *DB::MongoDB::QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayListNN<Text::String> *columNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition)
{
	return 0;
}

DB::TableDef *DB::MongoDB::GetTableDef(Text::CString schemaName, Text::CString tableName)
{
	return 0;
}
		
void DB::MongoDB::CloseReader(NotNullPtr<DBReader> r)
{
}

void DB::MongoDB::GetLastErrorMsg(NotNullPtr<Text::StringBuilderUTF8> str)
{
}

void DB::MongoDB::Reconnect()
{

}

UOSInt DB::MongoDB::GetDatabaseNames(NotNullPtr<Data::ArrayListNN<Text::String>> names)
{
	return 0;
}

void DB::MongoDB::FreeDatabaseNames(NotNullPtr<Data::ArrayListNN<Text::String>> names)
{
}

void DB::MongoDB::BuildURL(NotNullPtr<Text::StringBuilderUTF8> out, Text::CString userName, Text::CString password, Text::CString host, UInt16 port)
{
}

