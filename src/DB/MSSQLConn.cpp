#include "Stdafx.h"
#include "DB/MSSQLConn.h"
#include "DB/ODBCConn.h"
#include "Text/MyString.h"

DB::DBConn *DB::MSSQLConn::OpenConnTCP(const UTF8Char *serverHost, UInt16 port, const UTF8Char *database, const UTF8Char *userName, const UTF8Char *password, IO::LogTool *log, Text::StringBuilderUTF8 *errMsg)
{
	DB::ODBCConn *conn;
	Text::String *driverName = GetDriverNameNew();
	if (driverName == 0)
	{
		return 0;
	}
	if (port == 0)
	{
		port = 1433;
	}
	Text::StringBuilderUTF8 connStr;
	connStr.AppendC(UTF8STRC("Driver={"));
	connStr.Append(driverName);
	connStr.AppendC(UTF8STRC("}"));
	if (driverName->Equals(UTF8STRC("ODBC Driver 17 for SQL Server")))
	{
		connStr.AppendC(UTF8STRC(";server=tcp:"));
		connStr.AppendSlow(serverHost);
		connStr.AppendC(UTF8STRC(","));
		connStr.AppendU16(port);
	}
	else
	{
		connStr.AppendC(UTF8STRC(";server="));
		connStr.AppendSlow(serverHost);
		connStr.AppendC(UTF8STRC(";port="));
		connStr.AppendU16(port);
	}
	connStr.AppendC(UTF8STRC(";database="));
	connStr.AppendSlow(database);
	if (driverName->Equals(UTF8STRC("ODBC Driver 17 for SQL Server")))
	{
		if (userName)
		{
			connStr.AppendC(UTF8STRC(";UID="));
			connStr.AppendSlow(userName);
		}
		if (password)
		{
			connStr.AppendC(UTF8STRC(";PWD="));
			connStr.AppendSlow(password);
		}
	}
	else
	{
		if (userName)
		{
			connStr.AppendC(UTF8STRC(";user="));
			connStr.AppendSlow(userName);
		}
		if (password)
		{
			connStr.AppendC(UTF8STRC(";password="));
			connStr.AppendSlow(password);
		}
	}
	driverName->Release();
	NEW_CLASS(conn, DB::ODBCConn(connStr.ToCString(), CSTR("MSSQLConn"), log));
	if (conn->GetConnError() == DB::ODBCConn::CE_NONE)
	{
		return conn;
	}
	else
	{
		if (errMsg)
		{
			conn->GetErrorMsg(errMsg);
		}
		DEL_CLASS(conn);
		return 0;
	}
}

DB::DBTool *DB::MSSQLConn::CreateDBToolTCP(const UTF8Char *serverHost, UInt16 port, const UTF8Char *database, const UTF8Char *userName, const UTF8Char *password, IO::LogTool *log, Text::CString logPrefix)
{
	DB::DBConn *conn = OpenConnTCP(serverHost, port, database, userName, password, log, 0);
	DB::DBTool *db;
	if (conn)
	{
		NEW_CLASS(db, DB::DBTool(conn, true, log, logPrefix));
		return db;
	}
	else
	{
		DEL_CLASS(conn);
		return 0;
	}
}

Text::String *DB::MSSQLConn::GetDriverNameNew()
{
	Text::String *driverName = 0;
	Text::String *driver;
	Data::ArrayList<Text::String *> driverList;
	UOSInt i = 0;
	UOSInt j = DB::ODBCConn::GetDriverList(&driverList);
	while (i < j)
	{
		driver = driverList.GetItem(i);
		if (driver->Equals(UTF8STRC("ODBC Driver 17 for SQL Server")))
		{
			SDEL_STRING(driverName);
			driverName = driver;
		}
		else if (driver->Equals(UTF8STRC("ODBC Driver 13 for SQL Server")))
		{
			SDEL_STRING(driverName);
			driverName = driver;
		}
		else if (driver->Equals(UTF8STRC("ODBC Driver 11 for SQL Server")))
		{
			SDEL_STRING(driverName);
			driverName = driver;
		}
		else if (driver->Equals(UTF8STRC("FreeTDS")))
		{
			SDEL_STRING(driverName);
			driverName = driver;
		}
		else
		{
			driver->Release();
		}
		i++;
	}
	return driverName;
}
