#include "Stdafx.h"
#include "Core/Core.h"
#include "DB/DBReader.h"
#include "DB/TDSConn.h"
#include "IO/ConsoleLogHandler.h"

#define DBHOST CSTR("127.0.0.1")
#define DBPORT 1433
#define DBNAME CSTR("master")
#define DBUSER CSTR("user")
#define DBPASSWORD CSTR("password")

Int32 MyMain(Core::IProgControl *progCtrl)
{
	IO::ConsoleWriter console;
	IO::ConsoleLogHandler logHdlr(&console);
	IO::LogTool log;
	log.AddLogHandler(&logHdlr, IO::LogHandler::LogLevel::Raw);
	Text::StringBuilderUTF8 errMsg;
	DB::TDSConn *conn;
	NEW_CLASS(conn, DB::TDSConn(
		DBHOST,
		DBPORT,
		true,
		DBNAME,
		DBUSER,
		DBPASSWORD,
		&log,
		&errMsg
	));
	if (conn->IsConnected())
	{
		DB::DBReader *r = conn->ExecuteReader(CSTR("select SYSDATETIME(), GETUTCDATE()"));
		if (r)
		{
			if (r->ReadNext())
			{
				Text::StringBuilderUTF8 sb;
				Data::Timestamp ts1 = r->GetTimestamp(0);
				Data::Timestamp ts2 = r->GetTimestamp(1);
				sb.AppendTS(ts1);
				sb.AppendC(UTF8STRC(" - "));
				sb.AppendTS(ts2);
				console.WriteLineCStr(sb.ToCString());
			}
			conn->CloseReader(r);
		}
	}
	DEL_CLASS(conn);
	return 0;
}
