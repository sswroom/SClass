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

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	Net::WebServer::WebListener *svr;
	Text::StringBuilderUTF8 sb;
	UInt16 port;
	Text::CStringNN path;

#if defined(DEBUGCON)
	path = CSTR("/");
	NEW_CLASS(console, IO::DebugWriter());
#else
	path = CSTR(".");
	NEW_CLASS(console, IO::ConsoleWriter());
#endif

#if defined(HTTPPORT)
	port = HTTPPORT;
#else
	port = 80;
#endif

	UOSInt argc;
	UTF8Char **argv = progCtrl->GetCommandLines(progCtrl, argc);
	if (argc >= 2)
	{
		port = 0;
		Text::StrToUInt16(argv[1], port);
	}

	sb.ClearStr();
	sb.AppendC(UTF8STRC("Listening to port "));
	sb.AppendI32(port);
	console->WriteLine(sb.ToCString());
	Net::OSSocketFactory sockf(true);
	NN<Net::WebServer::WebStandardHandler> hdlr;
	NEW_CLASSNN(hdlr, Net::WebServer::HTTPDirectoryHandler(path, true, 65536, true));
	NEW_CLASS(svr, Net::WebServer::WebListener(sockf, 0, hdlr, port, 120, 1, 8, CSTR("sswr/1.0"), false, Net::WebServer::KeepAlive::Default, true));
	if (!svr->IsError())
	{
		progCtrl->WaitForExit(progCtrl);
	}
	else
	{
		console->WriteLine(CSTR("Error in listening port"));
	}
	DEL_CLASS(svr);
	hdlr.Delete();
	DEL_CLASS(console);

	return 0;
}
