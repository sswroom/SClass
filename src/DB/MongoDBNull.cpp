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

UOSInt DB::MongoDB::QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names)
{
	return 0;
}

Optional<DB::DBReader> DB::MongoDB::QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayListStringNN *columNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition)
{
	return 0;
}

DB::TableDef *DB::MongoDB::GetTableDef(Text::CString schemaName, Text::CString tableName)
{
	return 0;
}
		
void DB::MongoDB::CloseReader(NN<DBReader> r)
{
}

void DB::MongoDB::GetLastErrorMsg(NN<Text::StringBuilderUTF8> str)
{
}

void DB::MongoDB::Reconnect()
{

}

UOSInt DB::MongoDB::GetDatabaseNames(NN<Data::ArrayListStringNN> names)
{
	return 0;
}

void DB::MongoDB::FreeDatabaseNames(NN<Data::ArrayListStringNN> names)
{
}

void DB::MongoDB::BuildURL(NN<Text::StringBuilderUTF8> out, Text::CString userName, Text::CString password, Text::CString host, UInt16 port)
{
}

