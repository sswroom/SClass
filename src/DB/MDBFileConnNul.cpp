#include "Stdafx.h"
#include "DB/DBTool.h"
#include "DB/MDBFileConn.h"
#include "Text/Encoding.h"
#include "Text/EncodingFactory.h"
#include "Text/Locale.h"


DB::MDBFileConn::MDBFileConn(Text::CStringNN fileName, IO::LogTool *log, UInt32 codePage, const WChar *uid, const WChar *pwd) : DB::ODBCConn(fileName, log)
{
}

Bool DB::MDBFileConn::CreateMDBFile(Text::CStringNN fileName)
{
	return false;
}

DB::DBTool *DB::MDBFileConn::CreateDBTool(NotNullPtr<Text::String> fileName, IO::LogTool *log, Text::CString logPrefix)
{
	return 0;
}

DB::DBTool *DB::MDBFileConn::CreateDBTool(Text::CStringNN fileName, IO::LogTool *log, Text::CString logPrefix)
{
	return 0;
}

