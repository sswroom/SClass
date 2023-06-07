#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Net/LogServer.h"
#include "Net/OSSocketFactory.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/XML.h"

IO::ConsoleWriter *console;

class MyLogHandler : public IO::LogHandler
{
public:
	MyLogHandler()
	{
	}

	virtual ~MyLogHandler()
	{
	}

	void LogAdded(const Data::Timestamp &logTime, Text::CString logMsg, LogLevel logLev)
	{
		console->WriteLineCStr(logMsg);
	}

	void LogClosed()
	{

	}
};

Int32 MyMain(Core::IProgControl *progCtrl)
{
	Net::LogServer *svr;
	Net::SocketFactory *sockf;
	IO::LogTool *log;
	Text::StringBuilderUTF8 sb;
	MyLogHandler *logHdlr;
	NEW_CLASS(console, IO::ConsoleWriter());
	UInt16 port = 1234;

	UOSInt argc;
	UTF8Char **argv = progCtrl->GetCommandLines(progCtrl, &argc);
	if (argc >= 2)
	{
		Text::StrToUInt16S(argv[1], &port, 0);
	}

	sb.ClearStr();
	sb.AppendC(UTF8STRC("Listening to port "));
	sb.AppendI32(port);
	console->WriteLineC(sb.ToString(), sb.GetLength());
	NEW_CLASS(sockf, Net::OSSocketFactory(true));
	NEW_CLASS(log, IO::LogTool());
	NEW_CLASS(logHdlr, MyLogHandler());
	log->AddLogHandler(logHdlr, IO::LogHandler::LogLevel::Raw);
	NEW_CLASS(svr, Net::LogServer(sockf, port, CSTR("logs"), log, true, true));
	if (!svr->IsError())
	{
		progCtrl->WaitForExit(progCtrl);
	}
	else
	{
		console->WriteLineC(UTF8STRC("Error in listening port"));
	}
	DEL_CLASS(svr);
	DEL_CLASS(log);
	DEL_CLASS(logHdlr);
	DEL_CLASS(sockf);
	DEL_CLASS(console);
	return 0;
}
