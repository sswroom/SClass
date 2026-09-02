#include "Stdafx.h"
#include "DB/ODBCConn.h"
#include "DB/PostgreSQLConn.h"
#include "DB/PostgreSQLUtil.h"

Optional<DB::DBConn> DB::PostgreSQLUtil::OpenConn(Text::CStringNN serverHost, UInt16 port, Bool encrypt, Text::CString database, Text::CString userName, Text::CString password, NN<IO::LogTool> log, Optional<Text::StringBuilderUTF8> errMsg, Bool continueOnConnError)
{
	Text::CStringNN nnuserName;
	Text::CStringNN nnpassword;
	if (PostgreSQLConn::IsSupported() && userName.SetTo(nnuserName) && password.SetTo(nnpassword))
	{
		NN<DB::PostgreSQLConn> conn;
		NEW_CLASSNN(conn, DB::PostgreSQLConn(serverHost, port, nnuserName, nnpassword, database.Or(CSTR("postgres")), log));
		if (continueOnConnError || !conn->IsConnError())
			return conn;
		conn.Delete();
		return nullptr;
	}
	else
	{
		NN<DB::ODBCConn> conn;
		NN<Text::String> driverName;
		if (!GetDriverNameNew().SetTo(driverName))
		{
			log->LogMessage(CSTR("No suitable PostgreSQL driver found"), IO::LogHandler::LogLevel::Raw);
			return nullptr;
		}
		if (port == 0)
		{
			port = 5432;
		}
		Text::StringBuilderUTF8 connStr;
		connStr.AppendC(UTF8STRC("Driver={"));
		connStr.Append(driverName);
		connStr.AppendC(UTF8STRC("}"));
		connStr.AppendC(UTF8STRC(";Server="));
		connStr.Append(serverHost);
		connStr.AppendC(UTF8STRC(";Port="));
		connStr.AppendU16(port);
		if (encrypt)
		{
			connStr.AppendC(UTF8STRC(";sslmode=require"));
		}
		connStr.AppendC(UTF8STRC(";Database="));
		connStr.AppendOpt(database);
		if (userName.SetTo(nnuserName))
		{
			connStr.AppendC(UTF8STRC(";Uid="));
			connStr.Append(nnuserName);
		}
		if (password.SetTo(nnpassword))
		{
			connStr.AppendC(UTF8STRC(";Pwd="));
			connStr.Append(nnpassword);
		}
		connStr.AppendC(UTF8STRC(";connect_timeout=30"));
		{
			Text::StringBuilderUTF8 sb;
			sb.Append(CSTR("Using Driver "));
			sb.Append(driverName);
			log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Raw);
		}
		driverName->Release();
		NEW_CLASSNN(conn, DB::ODBCConn(connStr.ToCString(), CSTR("PostgreSQLConn"), log));
		if (conn->GetConnError() == DB::ODBCConn::CE_NONE || (continueOnConnError && conn->GetConnError() == DB::ODBCConn::CE_CONNECT_ERR))
		{
			return conn;
		}
		else
		{
			NN<Text::StringBuilderUTF8> sb;
			if (errMsg.SetTo(sb))
			{
				conn->GetLastErrorMsg(sb);
			}
			else
			{
				Text::StringBuilderUTF8 sbErr;
				conn->GetLastErrorMsg(sbErr);
				log->LogMessage(sbErr.ToCString(), IO::LogHandler::LogLevel::Error);
				log->LogMessage(connStr.ToCString(), IO::LogHandler::LogLevel::ErrorDetail);
			}
			conn.Delete();
			return nullptr;
		}
	}
}

Optional<DB::DBTool> DB::PostgreSQLUtil::CreateDBTool(Text::CStringNN serverHost, UInt16 port, Bool encrypt, Text::CString database, Text::CString userName, Text::CString password, NN<IO::LogTool> log, Text::CString logPrefix, Bool continueOnConnError)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendOpt(logPrefix);
	sb.AppendC(UTF8STRC("Error in connecting to database: "));
	NN<DB::DBTool> db;
	NN<DB::DBConn> conn;
	if (OpenConn(serverHost, port, encrypt, database, userName, password, log, sb, continueOnConnError).SetTo(conn))
	{
		NEW_CLASSNN(db, DB::DBTool(conn, true, log, logPrefix));
		return db;
	}
	else
	{
		log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Error);
		return nullptr;
	}
}

Optional<Text::String> DB::PostgreSQLUtil::GetDriverNameNew()
{
	if (PostgreSQLConn::IsSupported())
	{
		return Text::String::New(UTF8STRC("FreeTDS (Native)"));
	}
	Optional<Text::String> driverName = nullptr;
	NN<Text::String> driver;
	Data::ArrayListStringNN driverList;
	DB::ODBCConn::GetDriverList(driverList);
	Data::ArrayIterator<NN<Text::String>> it = driverList.Iterator();
	while (it.HasNext())
	{
		driver = it.Next();
		if (driver->StartsWith(UTF8STRC("PostgreSQL ")))
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
