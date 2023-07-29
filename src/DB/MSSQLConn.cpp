#include "Stdafx.h"
#include "DB/MSSQLConn.h"
#include "DB/ODBCConn.h"
#include "DB/TDSConn.h"
#include "Text/MyString.h"

DB::DBConn *DB::MSSQLConn::OpenConnTCP(Text::CString serverHost, UInt16 port, Bool encrypt, Text::CString database, Text::CString userName, Text::CString password, IO::LogTool *log, Text::StringBuilderUTF8 *errMsg)
{
	if (IsNative())
	{
		DB::TDSConn *conn;
		NEW_CLASS(conn, DB::TDSConn(serverHost, port, encrypt, database, userName, password, log, errMsg));
		if (conn->IsConnected())
			return conn;
		DEL_CLASS(conn);
		return 0;
	}
	else
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
			connStr.AppendC(UTF8STRC(";server="));
			connStr.Append(serverHost);
			connStr.AppendC(UTF8STRC(","));
			connStr.AppendU16(port);
			if (encrypt)
			{
				connStr.AppendC(UTF8STRC(";Encrypt=yes"));
			}
		}
		else
		{
			connStr.AppendC(UTF8STRC(";Server="));
			connStr.Append(serverHost);
			connStr.AppendC(UTF8STRC(";Port="));
			connStr.AppendU16(port);
			if (encrypt)
			{
				connStr.AppendC(UTF8STRC(";Encryption=require"));
			}
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
			connStr.AppendC(UTF8STRC(";TrustServerCertificate=YES"));	
		}
		else
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
	//		connStr.AppendC(UTF8STRC(";Timeout=60"));
	//		connStr.AppendC(UTF8STRC(";connect timeout=30"));
		}
		driverName->Release();
		NEW_CLASS(conn, DB::ODBCConn(connStr.ToCString(), CSTR("MSSQLConn"), log));
		if (conn->GetConnError() == DB::ODBCConn::CE_NONE)
		{
			return conn;
		}
		else
		{
			NotNullPtr<Text::StringBuilderUTF8> sb;
			if (sb.Set(errMsg))
			{
				conn->GetLastErrorMsg(sb);
			}
			DEL_CLASS(conn);
			return 0;
		}
	}
}

DB::DBTool *DB::MSSQLConn::CreateDBToolTCP(Text::CString serverHost, UInt16 port, Bool encrypt, Text::CString database, Text::CString userName, Text::CString password, IO::LogTool *log, Text::CString logPrefix)
{
	Text::StringBuilderUTF8 sb;
	sb.Append(logPrefix);
	sb.AppendC(UTF8STRC("Error in connecting to database: "));
	DB::DBConn *conn = OpenConnTCP(serverHost, port, encrypt, database, userName, password, log, &sb);
	DB::DBTool *db;
	if (conn)
	{
		NEW_CLASS(db, DB::DBTool(conn, true, log, logPrefix));
		return db;
	}
	else
	{
		if (log)
		{
			log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Error);
		}
		return 0;
	}
}

Text::String *DB::MSSQLConn::GetDriverNameNew()
{
	if (IsNative())
	{
		return Text::String::New(UTF8STRC("FreeTDS (Native)")).Ptr();
	}
	Text::String *driverName = 0;
	Text::String *driver;
	Data::ArrayListNN<Text::String> driverList;
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

Bool DB::MSSQLConn::IsNative()
{
#if defined(WIN32)
	return false;
#else
	return true;
#endif
}
