#include "Stdafx.h"
#include "Data/DateTime.h"
#include "Text/MyString.h"
#include "DB/DBConn.h"

DB::DBConn::DBConn(Text::String *sourceName) : DB::ReadingDB(sourceName)
{
	this->lastDataError = DE_NO_ERROR;
}

DB::DBConn::DBConn(Text::CString sourceName) : DB::ReadingDB(sourceName)
{
	this->lastDataError = DE_NO_ERROR;
}

DB::DBConn::~DBConn()
{
}

DB::DBConn::DataError DB::DBConn::GetLastDataError()
{
	return this->lastDataError;
}

Bool DB::DBConn::IsFullConn()
{
	return true;
}
