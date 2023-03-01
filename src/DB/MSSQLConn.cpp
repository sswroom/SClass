#include "Stdafx.h"
#include "DB/MSSQLConn.h"
#include "DB/ODBCConn.h"
#include "Text/MyString.h"

DB::DBConn *DB::MSSQLConn::OpenConnTCP(Text::CString serverHost, UInt16 port, Text::CString database, Text::CString userName, Text::CString password, IO::LogTool *log, Text::StringBuilderUTF8 *errMsg)
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
	if (driverName->StartsWith(UTF8STRC("ODBC Driver ")) && driverName->EndsWith(UTF8STRC(" for SQL Server")))
	{
		connStr.AppendC(UTF8STRC(";server=tcp:"));
		connStr.Append(serverHost);
		connStr.AppendC(UTF8STRC(","));
		connStr.AppendU16(port);
	}
	else
	{
		connStr.AppendC(UTF8STRC(";server="));
		connStr.Append(serverHost);
		connStr.AppendC(UTF8STRC(";port="));
		connStr.AppendU16(port);
	}
	connStr.AppendC(UTF8STRC(";database="));
	connStr.Append(database);
	if (driverName->StartsWith(UTF8STRC("ODBC Driver ")) && driverName->EndsWith(UTF8STRC(" for SQL Server")))
	{
		if (userName.v)
		{
			connStr.AppendC(UTF8STRC(";UID="));
			connStr.Append(userName);
		}
		if (password.v)
		{
			connStr.AppendC(UTF8STRC(";PWD="));
			connStr.Append(password);
		}
	}
	else
	{
		if (userName.v)
		{
			connStr.AppendC(UTF8STRC(";user="));
			connStr.Append(userName);
		}
		if (password.v)
		{
			connStr.AppendC(UTF8STRC(";password="));
			connStr.Append(password);
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
			conn->GetLastErrorMsg(errMsg);
		}
		DEL_CLASS(conn);
		return 0;
	}
}

DB::DBTool *DB::MSSQLConn::CreateDBToolTCP(Text::CString serverHost, UInt16 port, Text::CString database, Text::CString userName, Text::CString password, IO::LogTool *log, Text::CString logPrefix)
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
		if (driver->StartsWith(UTF8STRC("ODBC Driver ")) && driver->EndsWith(UTF8STRC(" for SQL Server"))) //ODBC Driver 17 for SQL Server
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
