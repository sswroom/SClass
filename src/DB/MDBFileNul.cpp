#include "Stdafx.h"
#include "DB/DBTool.h"
#include "DB/MDBFile.h"
#include "Text/Encoding.h"
#include "Text/EncodingFactory.h"
#include "Text/Locale.h"


DB::MDBFile::MDBFile(const UTF8Char *fileName, IO::LogTool *log, UInt32 codePage, const WChar *uid, const WChar *pwd) : DB::ODBCConn(fileName, log)
{
}

Bool DB::MDBFile::CreateMDBFile(const UTF8Char *fileName)
{
	return false;
}

DB::DBTool *DB::MDBFile::CreateDBTool(const UTF8Char *fileName, IO::LogTool *log, const UTF8Char *logPrefix)
{
	return 0;
}

