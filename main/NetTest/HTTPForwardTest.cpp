#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Net/OSSocketFactory.h"
#include "Net/SSLEngineFactory.h"
#include "Net/WebServer/HTTPForwardHandler.h"
#include "Net/WebServer/WebListener.h"

IO::ConsoleWriter *console;

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
//	Text::CStringNN sslCert = CSTR("C:\\Progs\\VCClass\\keystore\\localhost.crt");
//	Text::CStringNN sslKey = CSTR("C:\\Progs\\VCClass\\keystore\\localhost.key");
	Text::CStringNN sslCert = CSTR("/home/sswroom/Progs/VCClass/keystore/localhost.crt");
	Text::CStringNN sslKey = CSTR("/home/sswroom/Progs/VCClass/keystore/localhost.key");
	Text::CStringNN fwdUrl = CSTR("https://192.168.0.196:8448/");
	UInt16 port = 12345;
	UTF8Char sbuff[512];
	UTF8Char *sptr;

	NEW_CLASS(console, IO::ConsoleWriter());
	Optional<Net::SSLEngine> ssl;
	Bool succ = true;
	Net::OSSocketFactory sockf(false);
	ssl = Net::SSLEngineFactory::Create(sockf, true);
	NN<Net::SSLEngine> nnssl;
	if (!ssl.SetTo(nnssl))
	{
		console->WriteLine(CSTR("Error in initializing SSL"));
		succ = false;
	}
	else if (!nnssl->ServerSetCerts(sslCert, sslKey))
	{
		console->WriteLine(CSTR("Error in loading Cert/Key"));
		sptr = nnssl->GetErrorDetail(sbuff);
		console->WriteLine(CSTRP(sbuff, sptr));
		succ = false;
	}

	if (succ)
	{
		NN<Net::WebServer::HTTPForwardHandler> hdlr;
		Net::WebServer::WebListener *svr;
		NEW_CLASSNN(hdlr, Net::WebServer::HTTPForwardHandler(sockf, ssl, fwdUrl, Net::WebServer::HTTPForwardHandler::ForwardType::Normal));
		NEW_CLASS(svr, Net::WebServer::WebListener(sockf, ssl, hdlr, port, 120, 1, 4, CSTR("sswr/1.0"), false, Net::WebServer::KeepAlive::Default, true));
		if (!svr->IsError())
		{
			console->WriteLine(CSTR("HTTP Forwarding started"));
			progCtrl->WaitForExit(progCtrl);
		}
		else
		{
			console->WriteLine(CSTR("Error in listening port"));
		}
		DEL_CLASS(svr);
		hdlr.Delete();
	}
	ssl.Delete();
	DEL_CLASS(console);
	return 0;
}
