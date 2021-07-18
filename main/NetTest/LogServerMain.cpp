#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Net/LogServer.h"
#include "Net/OSSocketFactory.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/XML.h"

IO::ConsoleWriter *console;

class MyLogHandler : public IO::ILogHandler
{
public:
	MyLogHandler()
	{
	}

	virtual ~MyLogHandler()
	{
	}

	void LogAdded(Data::DateTime *logTime, const UTF8Char *logMsg, LogLevel logLev)
	{
		console->WriteLine(logMsg);
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
	sb.Append((const UTF8Char*)"Listening to port ");
	sb.AppendI32(port);
	console->WriteLine(sb.ToString());
	NEW_CLASS(sockf, Net::OSSocketFactory(true));
	NEW_CLASS(log, IO::LogTool());
	NEW_CLASS(logHdlr, MyLogHandler());
	log->AddLogHandler(logHdlr, IO::ILogHandler::LOG_LEVEL_RAW);
	NEW_CLASS(svr, Net::LogServer(sockf, port, (const UTF8Char*)"logs", log, true));
	if (!svr->IsError())
	{
		progCtrl->WaitForExit(progCtrl);
	}
	else
	{
		console->WriteLine((const UTF8Char*)"Error in listening port");
	}
	DEL_CLASS(svr);
	DEL_CLASS(log);
	DEL_CLASS(logHdlr);
	DEL_CLASS(sockf);
	DEL_CLASS(console);
	return 0;
}
