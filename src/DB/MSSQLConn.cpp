#include "Stdafx.h"
#include "DB/MSSQLConn.h"
#include "DB/ODBCConn.h"
#include "Text/MyString.h"

DB::DBConn *DB::MSSQLConn::OpenConnTCP(const UTF8Char *serverHost, UInt16 port, const UTF8Char *database, const UTF8Char *userName, const UTF8Char *password, IO::LogTool *log, Text::StringBuilderUTF *errMsg)
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
	connStr.Append((const UTF8Char *)"Driver={");
	connStr.Append(driverName);
	connStr.Append((const UTF8Char*)"}");
	if (driverName->Equals((const UTF8Char*)"ODBC Driver 17 for SQL Server"))
	{
		connStr.Append((const UTF8Char*)";server=tcp:");
		connStr.Append(serverHost);
		connStr.Append((const UTF8Char*)",");
		connStr.AppendU16(port);
	}
	else
	{
		connStr.Append((const UTF8Char*)";server=");
		connStr.Append(serverHost);
		connStr.Append((const UTF8Char*)";port=");
		connStr.AppendU16(port);
	}
	connStr.Append((const UTF8Char*)";database=");
	connStr.Append(database);
	if (driverName->Equals((const UTF8Char*)"ODBC Driver 17 for SQL Server"))
	{
		if (userName)
		{
			connStr.Append((const UTF8Char*)";UID=");
			connStr.Append(userName);
		}
		if (password)
		{
			connStr.Append((const UTF8Char*)";PWD=");
			connStr.Append(password);
		}
	}
	else
	{
		if (userName)
		{
			connStr.Append((const UTF8Char*)";user=");
			connStr.Append(userName);
		}
		if (password)
		{
			connStr.Append((const UTF8Char*)";password=");
			connStr.Append(password);
		}
	}
	driverName->Release();
	NEW_CLASS(conn, DB::ODBCConn(connStr.ToString(), (const UTF8Char*)"MSSQLConn", log));
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

DB::DBTool *DB::MSSQLConn::CreateDBToolTCP(const UTF8Char *serverHost, UInt16 port, const UTF8Char *database, const UTF8Char *userName, const UTF8Char *password, IO::LogTool *log, const UTF8Char *logPrefix)
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
		if (driver->Equals((const UTF8Char*)"ODBC Driver 17 for SQL Server"))
		{
			SDEL_STRING(driverName);
			driverName = driver;
		}
		else if (driver->Equals((const UTF8Char*)"ODBC Driver 13 for SQL Server"))
		{
			SDEL_STRING(driverName);
			driverName = driver;
		}
		else if (driver->Equals((const UTF8Char*)"ODBC Driver 11 for SQL Server"))
		{
			SDEL_STRING(driverName);
			driverName = driver;
		}
		else if (driver->Equals((const UTF8Char*)"FreeTDS"))
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
