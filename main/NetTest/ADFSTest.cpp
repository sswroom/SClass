#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/Path.h"
#include "Net/OSSocketFactory.h"
#include "Net/SSLEngineFactory.h"
#include "Net/WebServer/PrintLogWebHandler.h"
#include "Net/WebServer/WebListener.h"
#include "Net/WebServer/WebServiceHandler.h"

IO::ConsoleWriter *console;
Net::SocketFactory *sockf;
Net::SSLEngine *ssl;
Net::WebServer::WebListener *listener;
Bool initSucc;
Net::WebServer::PrintLogWebHandler *logHdlr;

class MyADFSService : public Net::WebServer::WebServiceHandler
{
public:
	MyADFSService()
	{

	}

	virtual ~MyADFSService()
	{

	}
};

Int32 MyMain(Core::IProgControl *progCtrl)
{
	UTF8Char sbuff1[512];
	UTF8Char sbuff2[512];
	UTF8Char *sptr1;
	UTF8Char *sptr2;
	NEW_CLASS(console, IO::ConsoleWriter());
	NEW_CLASS(sockf, Net::OSSocketFactory(true));
	initSucc = false;
	ssl = Net::SSLEngineFactory::Create(sockf, false);
	if (ssl)
	{
		sptr1 = IO::Path::GetProcessFileName(sbuff1);
		sptr1 = IO::Path::AppendPathC(sbuff1, sptr1, UTF8STRC("ADFSCert.crt"));
		sptr2 = IO::Path::GetProcessFileName(sbuff2);
		sptr2 = IO::Path::AppendPathC(sbuff2, sptr2, UTF8STRC("ADFSCert.key"));
		if (ssl->SetServerCerts(CSTRP(sbuff1, sptr1), CSTRP(sbuff2, sptr2)))
		{
			initSucc = true;
		}
		else
		{
			console->WriteLineC(UTF8STRC("Error in loading ADFSCert.crt/ADFSCert.key file"));
		}
	}
	else
	{
		console->WriteLineC(UTF8STRC("Error in initializing SSL Engine"));
	}
	if (initSucc)
	{
		MyADFSService *svcHdlr;
		NEW_CLASS(svcHdlr, MyADFSService());
		NEW_CLASS(logHdlr, Net::WebServer::PrintLogWebHandler(svcHdlr, console));
		NEW_CLASS(listener, Net::WebServer::WebListener(sockf, ssl, logHdlr, 4321, 120, 4, (const UTF8Char*)"ADFSTest/1.0", false, true));
		if (listener->IsError())
		{
			console->WriteLineC(UTF8STRC("Error in listening to port 4321"));
		}
		else
		{
			console->WriteLineC(UTF8STRC("Listening to port 4321 (https)"));
			progCtrl->WaitForExit(progCtrl);
			console->WriteLineC(UTF8STRC("Server stopping"));
		}
		logHdlr->Release();
		DEL_CLASS(listener);
	}
	SDEL_CLASS(ssl);
	DEL_CLASS(sockf);
	DEL_CLASS(console);
	return 0;
}
