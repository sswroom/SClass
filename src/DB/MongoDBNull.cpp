#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/MongoDB.h"

Int32 DB::MongoDB::initCnt = 0;

DB::MongoDB::MongoDB(Text::CStringNN url, Text::CString database, Optional<IO::LogTool> log) : DB::ReadingDB(url)
{
}

DB::MongoDB::~MongoDB()
{
}

UIntOS DB::MongoDB::QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names)
{
	return 0;
}

Optional<DB::DBReader> DB::MongoDB::QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columNames, UIntOS ofst, UIntOS maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition)
{
	return nullptr;
}

Optional<DB::TableDef> DB::MongoDB::GetTableDef(Text::CString schemaName, Text::CStringNN tableName)
{
	return nullptr;
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

UIntOS DB::MongoDB::GetDatabaseNames(NN<Data::ArrayListStringNN> names)
{
	return 0;
}

void DB::MongoDB::FreeDatabaseNames(NN<Data::ArrayListStringNN> names)
{
}

void DB::MongoDB::BuildURL(NN<Text::StringBuilderUTF8> out, Text::CString userName, Text::CString password, Text::CStringNN host, UInt16 port)
{
}

