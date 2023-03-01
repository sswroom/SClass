#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/MongoDB.h"

Int32 DB::MongoDB::initCnt = 0;

DB::MongoDB::MongoDB(Text::CString url, Text::CString database, IO::LogTool *log) : DB::ReadingDB(url)
{
}

DB::MongoDB::~MongoDB()
{
}

UOSInt DB::MongoDB::QueryTableNames(Text::CString schemaName, Data::ArrayList<Text::String*> *names)
{
	return 0;
}

DB::DBReader *DB::MongoDB::QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayList<Text::String*> *columNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition)
{
	return 0;
}

DB::TableDef *DB::MongoDB::GetTableDef(Text::CString schemaName, Text::CString tableName)
{
	return 0;
}
		
void DB::MongoDB::CloseReader(DBReader *r)
{
}

void DB::MongoDB::GetLastErrorMsg(Text::StringBuilderUTF8 *str)
{
}

void DB::MongoDB::Reconnect()
{

}

UOSInt DB::MongoDB::GetDatabaseNames(Data::ArrayList<Text::String*> *names)
{
	return 0;
}

void DB::MongoDB::FreeDatabaseNames(Data::ArrayList<Text::String*> *names)
{
}

void DB::MongoDB::BuildURL(Text::StringBuilderUTF8 *out, Text::CString userName, Text::CString password, Text::CString host, UInt16 port)
{
}

