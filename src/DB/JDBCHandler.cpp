#include "Stdafx.h"
#include "DB/JDBCHandler.h"
#include "DB/MSSQLConn.h"

#define LOGPREFIX ((const UTF8Char*)"DB:")

DB::DBTool *DB::JDBCHandler::OpenConn(Text::String *url, Text::String *username, Text::String *password, IO::LogTool *log, Net::SocketFactory *sockf)
{
	if (url == 0 || !url->StartsWith((const UTF8Char*)"jdbc:"))
	{
		return 0;
	}
	if (url->StartsWith(5, (const UTF8Char*)"sqlserver://"))
	{
		Text::StringBuilderUTF8 sb;
		UTF8Char *sarr[2];
		UTF8Char *sarr2[2];
		UOSInt scnt;
		UOSInt scnt2;
		UInt16 port = 1433;
		const UTF8Char *dbName = 0;
		sb.AppendC(&url->v[17], url->leng - 17);
		sarr[1] = sb.ToString();
		scnt = Text::StrSplit(sarr, 2, sarr[1], ';');
		scnt2 = Text::StrSplit(sarr2, 2, sarr[0], ':');
		if (scnt == 2)
		{
			if (!Text::StrToUInt16(sarr2[1], &port))
			{
				return 0;
			}
		}
		while (scnt == 2)
		{
			scnt = Text::StrSplit(sarr, 2, sarr[1], ';');
			if (Text::StrStartsWithICase(sarr[0], (const UTF8Char*)"databaseName="))
			{
				dbName = &sarr[0][13];
			}
		}
		return MSSQLConn::CreateDBToolTCP(sarr2[0], port, dbName, STR_PTR(username), STR_PTR(password), log, LOGPREFIX);
	}
	return 0;
}
