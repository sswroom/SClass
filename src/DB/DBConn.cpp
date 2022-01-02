#include "Stdafx.h"
#include "Data/DateTime.h"
#include "Text/MyString.h"
#include "DB/DBConn.h"

DB::DBConn::DBConn(Text::String *sourceName) : DB::ReadingDB(sourceName)
{
	this->lastDataError = DE_NO_ERROR;
}

DB::DBConn::DBConn(const UTF8Char *sourceName) : DB::ReadingDB(sourceName)
{
	this->lastDataError = DE_NO_ERROR;
}

DB::DBConn::~DBConn()
{
}

OSInt DB::DBConn::ExecuteNonQuery(const UTF8Char *sql, UOSInt sqlLen)
{
	return ExecuteNonQuery(sql);
}

DB::DBReader *DB::DBConn::ExecuteReader(const UTF8Char *sql, UOSInt sqlLen)
{
	return ExecuteReader(sql);
}

DB::DBConn::DataError DB::DBConn::GetLastDataError()
{
	return this->lastDataError;
}

Bool DB::DBConn::IsFullConn()
{
	return true;
}
