#include "Stdafx.h"
#include "DB/DBTool.h"
#include "DB/MDBFileConn.h"
#include "Text/Encoding.h"
#include "Text/EncodingFactory.h"
#include "Text/Locale.h"


DB::MDBFileConn::MDBFileConn(const UTF8Char *fileName, IO::LogTool *log, UInt32 codePage, const WChar *uid, const WChar *pwd) : DB::ODBCConn(fileName, log)
{
}

Bool DB::MDBFileConn::CreateMDBFile(const UTF8Char *fileName)
{
	return false;
}

DB::DBTool *DB::MDBFileConn::CreateDBTool(Text::String *fileName, IO::LogTool *log, const UTF8Char *logPrefix)
{
	return 0;
}

DB::DBTool *DB::MDBFileConn::CreateDBTool(const UTF8Char *fileName, IO::LogTool *log, const UTF8Char *logPrefix)
{
	return 0;
}

