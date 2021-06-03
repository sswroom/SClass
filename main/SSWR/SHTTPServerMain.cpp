#include "Stdafx.h"
#include "Core/Core.h"
#if defined(DEBUGCON)
#include "IO/DebugWriter.h"
#else
#include "IO/ConsoleWriter.h"
#endif
#include "Net/OSSocketFactory.h"
#include "Net/WebServer/HTTPDirectoryHandler.h"
#include "Net/WebServer/HTTPFormParser.h"
#include "Net/WebServer/WebListener.h"
#include "Net/WebServer/WebStandardHandler.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/XML.h"

IO::Writer *console;

Int32 MyMain(Core::IProgControl *progCtrl)
{
	Net::WebServer::WebListener *svr;
	Net::SocketFactory *sockf;
	Net::WebServer::WebStandardHandler *hdlr;
	Text::StringBuilderUTF8 sb;
	UInt16 port;
	const UTF8Char *path;

#if defined(DEBUGCON)
	path = (const UTF8Char *)"/";
	NEW_CLASS(console, IO::DebugWriter());
#else
	path = (const UTF8Char *)".";
	NEW_CLASS(console, IO::ConsoleWriter());
#endif

#if defined(HTTPPORT)
	port = HTTPPORT;
#else
	port = 80;
#endif

	OSInt argc;
	UTF8Char **argv = progCtrl->GetCommandLines(progCtrl, &argc);
	if (argc >= 2)
	{
		port = 0;
		Text::StrToUInt16(argv[1], &port);
	}

	sb.ClearStr();
	sb.Append((const UTF8Char*)"Listening to port ");
	sb.AppendI32(port);
	console->WriteLine(sb.ToString());
	NEW_CLASS(sockf, Net::OSSocketFactory(true));
	NEW_CLASS(hdlr, Net::WebServer::HTTPDirectoryHandler(path, true, 65536, true));
	NEW_CLASS(svr, Net::WebServer::WebListener(sockf, hdlr, port, 120, 8, (const UTF8Char*)"sswr/1.0", false, true));
	if (!svr->IsError())
	{
		progCtrl->WaitForExit(progCtrl);
	}
	else
	{
		console->WriteLine((const UTF8Char*)"Error in listening port");
	}
	DEL_CLASS(svr);
	hdlr->Release();
	DEL_CLASS(sockf);
	DEL_CLASS(console);
	return 0;
}
