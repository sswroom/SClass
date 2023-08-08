#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Net/OSSocketFactory.h"
#include "Net/SSLEngineFactory.h"
#include "Net/WebServer/HTTPForwardHandler.h"
#include "Net/WebServer/WebListener.h"

IO::ConsoleWriter *console;

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
//	Text::CStringNN sslCert = CSTR("C:\\Progs\\VCClass\\keystore\\localhost.crt");
//	Text::CStringNN sslKey = CSTR("C:\\Progs\\VCClass\\keystore\\localhost.key");
	Text::CStringNN sslCert = CSTR("/home/sswroom/Progs/VCClass/keystore/localhost.crt");
	Text::CStringNN sslKey = CSTR("/home/sswroom/Progs/VCClass/keystore/localhost.key");
	Text::CString fwdUrl = CSTR("https://192.168.0.196:8448/");
	UInt16 port = 12345;
	UTF8Char sbuff[512];
	UTF8Char *sptr;

	NEW_CLASS(console, IO::ConsoleWriter());
	Net::SSLEngine *ssl;
	Bool succ = true;
	Net::OSSocketFactory sockf(false);
	ssl = Net::SSLEngineFactory::Create(sockf, true);
	if (ssl == 0)
	{
		console->WriteLineC(UTF8STRC("Error in initializing SSL"));
		succ = false;
	}
	else if (!ssl->ServerSetCerts(sslCert, sslKey))
	{
		console->WriteLineC(UTF8STRC("Error in loading Cert/Key"));
		sptr = ssl->GetErrorDetail(sbuff);
		console->WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
		succ = false;
	}

	if (succ)
	{
		Net::WebServer::HTTPForwardHandler *hdlr;
		Net::WebServer::WebListener *svr;
		NEW_CLASS(hdlr, Net::WebServer::HTTPForwardHandler(sockf, ssl, fwdUrl, Net::WebServer::HTTPForwardHandler::ForwardType::Normal));
		NEW_CLASS(svr, Net::WebServer::WebListener(sockf, ssl, hdlr, port, 120, 4, CSTR("sswr/1.0"), false, Net::WebServer::KeepAlive::Default, true));
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
	DEL_CLASS(console);
	return 0;
}
