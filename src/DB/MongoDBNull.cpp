#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/MongoDB.h"
\
Int32 DB::MongoDB::initCnt = 0;

DB::MongoDB::MongoDB(const UTF8Char *url, const UTF8Char *database, IO::LogTool *log) : DB::ReadingDB(url)
{
}

DB::MongoDB::~MongoDB()
{
}

UOSInt DB::MongoDB::GetTableNames(Data::ArrayList<const UTF8Char*> *names)
{
	return 0;
}

DB::DBReader *DB::MongoDB::GetTableData(const UTF8Char *name, Data::ArrayList<const UTF8Char*> *columNames, UOSInt ofst, UOSInt maxCnt, const UTF8Char *ordering, Data::QueryConditions *condition)
{
	return 0;
}

void DB::MongoDB::CloseReader(DBReader *r)
{
}

void DB::MongoDB::GetErrorMsg(Text::StringBuilderUTF *str)
{
}

void DB::MongoDB::Reconnect()
{

}

UOSInt DB::MongoDB::GetDatabaseNames(Data::ArrayList<const UTF8Char*> *names)
{
	return 0;
}

void DB::MongoDB::FreeDatabaseNames(Data::ArrayList<const UTF8Char*> *names)
{
}

void DB::MongoDB::BuildURL(Text::StringBuilderUTF *out, const UTF8Char *userName, const UTF8Char *password, const UTF8Char *host, UInt16 port)
{
}

