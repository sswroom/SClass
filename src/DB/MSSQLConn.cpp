#include "Stdafx.h"
#include "DB/MSSQLConn.h"
#include "DB/ODBCConn.h"
#include "DB/TDSConn.h"
#include "Text/MyString.h"

Optional<DB::DBConn> DB::MSSQLConn::OpenConnTCP(Text::CStringNN serverHost, UInt16 port, Bool encrypt, Text::CString database, Text::CString userName, Text::CString password, NN<IO::LogTool> log, Text::StringBuilderUTF8 *errMsg)
{
	Text::CStringNN nns;
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
		NN<Text::String> driverName;
		if (!GetDriverNameNew().SetTo(driverName))
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
		connStr.AppendOpt(database);
		if (driverName->StartsWith(UTF8STRC("ODBC Driver ")) && driverName->EndsWith(UTF8STRC(" for SQL Server")))
		{
			if (userName.SetTo(nns))
			{
				connStr.AppendC(UTF8STRC(";UID="));
				connStr.Append(nns);
			}
			if (password.SetTo(nns))
			{
				connStr.AppendC(UTF8STRC(";PWD="));
				connStr.Append(nns);
			}
			connStr.AppendC(UTF8STRC(";TrustServerCertificate=YES"));	
		}
		else
		{
			if (userName.SetTo(nns))
			{
				connStr.AppendC(UTF8STRC(";UID="));
				connStr.Append(nns);
			}
			if (password.SetTo(nns))
			{
				connStr.AppendC(UTF8STRC(";PWD="));
				connStr.Append(nns);
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
			NN<Text::StringBuilderUTF8> sb;
			if (sb.Set(errMsg))
			{
				conn->GetLastErrorMsg(sb);
			}
			DEL_CLASS(conn);
			return 0;
		}
	}
}

Optional<DB::DBTool> DB::MSSQLConn::CreateDBToolTCP(Text::CStringNN serverHost, UInt16 port, Bool encrypt, Text::CString database, Text::CString userName, Text::CString password, NN<IO::LogTool> log, Text::CString logPrefix)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendOpt(logPrefix);
	sb.AppendC(UTF8STRC("Error in connecting to database: "));
	NN<DB::DBTool> db;
	NN<DB::DBConn> conn;
	if (OpenConnTCP(serverHost, port, encrypt, database, userName, password, log, &sb).SetTo(conn))
	{
		NEW_CLASSNN(db, DB::DBTool(conn, true, log, logPrefix));
		return db;
	}
	else
	{
		log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Error);
		return 0;
	}
}

Optional<Text::String> DB::MSSQLConn::GetDriverNameNew()
{
	if (IsNative())
	{
		return Text::String::New(UTF8STRC("FreeTDS (Native)"));
	}
	Optional<Text::String> driverName = 0;
	NN<Text::String> driver;
	Data::ArrayListStringNN driverList;
	DB::ODBCConn::GetDriverList(driverList);
	Data::ArrayIterator<NN<Text::String>> it = driverList.Iterator();
	while (it.HasNext())
	{
		driver = it.Next();
		if (driver->StartsWith(UTF8STRC("ODBC Driver ")) && driver->EndsWith(UTF8STRC(" for SQL Server"))) //ODBC Driver 17 for SQL Server
		{
			OPTSTR_DEL(driverName);
			driverName = driver;
		}
		else if (driver->Equals(UTF8STRC("FreeTDS")))
		{
			OPTSTR_DEL(driverName);
			driverName = driver;
		}
		else
		{
			driver->Release();
		}
	}
	return driverName;
}

Bool DB::MSSQLConn::IsNative()
{
#if defined(WIN32) && !defined(__CYGWIN__)
	return false;
#else
	return true;
#endif
}
