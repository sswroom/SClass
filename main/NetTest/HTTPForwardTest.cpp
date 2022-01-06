#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Net/OSSocketFactory.h"
#include "Net/SSLEngineFactory.h"
#include "Net/WebServer/HTTPForwardHandler.h"
#include "Net/WebServer/WebListener.h"

IO::ConsoleWriter *console;

Int32 MyMain(Core::IProgControl *progCtrl)
{
//	const UTF8Char *sslCert = (const UTF8Char*)"C:\\Progs\\VCClass\\keystore\\localhost.crt";
//	const UTF8Char *sslKey = (const UTF8Char*)"C:\\Progs\\VCClass\\keystore\\localhost.key";
	const UTF8Char *sslCert = (const UTF8Char*)"/home/sswroom/Progs/VCClass/keystore/localhost.crt";
	const UTF8Char *sslKey = (const UTF8Char*)"/home/sswroom/Progs/VCClass/keystore/localhost.key";
	const UTF8Char *fwdUrl = (const UTF8Char*)"https://192.168.0.196:8448/";
	UInt16 port = 12345;

	NEW_CLASS(console, IO::ConsoleWriter());
	Net::SocketFactory *sockf;
	Net::SSLEngine *ssl;
	Bool succ = true;
	NEW_CLASS(sockf, Net::OSSocketFactory(false));
	ssl = Net::SSLEngineFactory::Create(sockf, true);
	if (ssl == 0)
	{
		UTF8Char sbuff[512];
		console->WriteLineC(UTF8STRC("Error in initializing SSL"));
		ssl->GetErrorDetail(sbuff);
		console->WriteLine(sbuff);
		succ = false;
	}
	else if (!ssl->SetServerCerts(sslCert, sslKey))
	{
		UTF8Char sbuff[512];
		console->WriteLineC(UTF8STRC("Error in loading Cert/Key"));
		ssl->GetErrorDetail(sbuff);
		console->WriteLine(sbuff);
		succ = false;
	}

	if (succ)
	{
		Net::WebServer::HTTPForwardHandler *hdlr;
		Net::WebServer::WebListener *svr;
		NEW_CLASS(hdlr, Net::WebServer::HTTPForwardHandler(sockf, ssl, fwdUrl, Net::WebServer::HTTPForwardHandler::ForwardType::Normal));
		NEW_CLASS(svr, Net::WebServer::WebListener(sockf, ssl, hdlr, port, 120, 4, (const UTF8Char*)"sswr/1.0", false, true));
		if (!svr->IsError())
		{
			console->WriteLineC(UTF8STRC("HTTP Forwarding started"));
			progCtrl->WaitForExit(progCtrl);
		}
		else
		{
			console->WriteLineC(UTF8STRC("Error in listening port"));
		}
		DEL_CLASS(svr);
		hdlr->Release();
	}
	SDEL_CLASS(ssl);
	DEL_CLASS(sockf);
	DEL_CLASS(console);
	return 0;
}
