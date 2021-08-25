#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Net/DefaultSSLEngine.h"
#include "Net/OSSocketFactory.h"
#include "Net/WebServer/HTTPDirectoryHandler.h"
#include "Net/WebServer/HTTPFormParser.h"
#include "Net/WebServer/WebListener.h"
#include "Net/WebServer/WebStandardHandler.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/XML.h"

IO::ConsoleWriter *console;

Int32 MyMain(Core::IProgControl *progCtrl)
{
	Net::WebServer::WebListener *svr;
	Net::SocketFactory *sockf;
	Net::SSLEngine *ssl;
	Net::WebServer::WebStandardHandler *hdlr;
	Text::StringBuilderUTF8 sb;
	IO::ConsoleWriter *console;
	UInt16 port = 80;
	NEW_CLASS(console, IO::ConsoleWriter());

	UOSInt argc;
	UTF8Char **argv = progCtrl->GetCommandLines(progCtrl, &argc);
	if (argc >= 2)
	{
		port = 0;
		Text::StrToUInt16(argv[1], &port);
		if (port <= 0)
		{
			port = 0;
		}
	}

	NEW_CLASS(sockf, Net::OSSocketFactory(true));
	ssl = Net::DefaultSSLEngine::Create(sockf, true);
	if (ssl == 0 || !ssl->SetServerCerts((const UTF8Char*)"test.crt", (const UTF8Char*)"test.key"))
	{
		UTF8Char sbuff[512];
		console->WriteLine((const UTF8Char*)"Error in initializing SSL");
		ssl->GetErrorDetail(sbuff);
		console->WriteLine(sbuff);
	}
	else
	{
		sb.ClearStr();
		sb.Append((const UTF8Char*)"Listening to port ");
		sb.AppendI32(port);
		console->WriteLine(sb.ToString());
		NEW_CLASS(hdlr, Net::WebServer::HTTPDirectoryHandler((const UTF8Char*)".", true, 0, true));
	//	NEW_CLASS(svr, Net::WebServer::WebListener(sockf, sslSvr, hdlr, port, 120, 8, (const UTF8Char*)"sswr", false, true));
		NEW_CLASS(svr, Net::WebServer::WebListener(sockf, ssl, hdlr, port, 120, 1, (const UTF8Char*)"sswr", false, true));
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
	}
	SDEL_CLASS(ssl);
	DEL_CLASS(sockf);
	DEL_CLASS(console);
	return 0;
}
