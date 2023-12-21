#include "Stdafx.h"
#include "Core/Core.h"
#include "DB/DBReader.h"
#include "DB/MSSQLConn.h"
#include "IO/ConsoleLogHandler.h"

#define DBHOST CSTR("127.0.0.1")
#define DBPORT 1433
#define DBNAME CSTR("master")
#define DBUSER CSTR("user")
#define DBPASSWORD CSTR("password")

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	IO::ConsoleWriter console;
	IO::ConsoleLogHandler logHdlr(console);
	IO::LogTool log;
	log.AddLogHandler(logHdlr, IO::LogHandler::LogLevel::Raw);
	Text::StringBuilderUTF8 errMsg;
	NotNullPtr<DB::DBConn> conn;
	if (DB::MSSQLConn::OpenConnTCP(
		DBHOST,
		DBPORT,
		true,
		DBNAME,
		DBUSER,
		DBPASSWORD,
		log,
		&errMsg
	).SetTo(conn))
	{
		NotNullPtr<DB::DBReader> r;
		if (r.Set(conn->ExecuteReader(CSTR("select * from table"))))
		{
			while (r->ReadNext())
			{
				Optional<Text::String> s;
				r->GetInt32(0);
				s = r->GetNewStr(1);
				OPTSTR_DEL(s);
				s = r->GetNewStr(2);
				OPTSTR_DEL(s);
				s = r->GetNewStr(3);
				OPTSTR_DEL(s);
				s = r->GetNewStr(4);
				OPTSTR_DEL(s);
				s = r->GetNewStr(5);
				OPTSTR_DEL(s);
				s = r->GetNewStr(6);
				OPTSTR_DEL(s);
				s = r->GetNewStr(7);
				OPTSTR_DEL(s);
				r->GetInt32(8);
				s = r->GetNewStr(9);
				OPTSTR_DEL(s);
				r->GetTimestamp(10);
				r->GetTimestamp(11);
				r->GetInt32(12);
				r->GetInt32(13);
				r->GetInt32(14);
			}
			conn->CloseReader(r);
		}
		conn.Delete();
	}
	else
	{
		console.WriteLineCStr(CSTR("Error in connecting to database"));
		console.WriteLineCStr(errMsg.ToCString());
	}
	return 0;
}
