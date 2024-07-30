#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Net/OSSocketFactory.h"
#include "Net/WebServer/OAuth2Handler.h"
#include "Net/WebServer/PrintLogWebHandler.h"
#include "Net/WebServer/WebListener.h"

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	IO::ConsoleWriter console;
	NN<Net::WebServer::OAuth2Handler> oauth2Hdlr;
	NN<Net::WebServer::PrintLogWebHandler> hdlr;
	Net::WebServer::WebListener *listener;
	Net::OSSocketFactory sockf(false);
	Net::TCPClientFactory clif(sockf);
	NEW_CLASSNN(oauth2Hdlr, Net::WebServer::OAuth2Handler(CSTR("/auth"), CSTR("/token"), CSTR("/userinfo")));
	NEW_CLASSNN(hdlr, Net::WebServer::PrintLogWebHandler(oauth2Hdlr, console));
	NEW_CLASS(listener, Net::WebServer::WebListener(clif, 0, hdlr, 8889, 30, 1, 4, CSTR("OAuthTest/1.0"), false, Net::WebServer::KeepAlive::Default, true));
	if (!listener->IsError())
	{
		console.WriteLine(CSTR("OAuth2 Test running at port 8889"));
		progCtrl->WaitForExit(progCtrl);
		console.WriteLine(CSTR("OAuth2 Test is stopping"));
	}
	else
	{
		console.WriteLine(CSTR("Error in listening to port 8889"));
	}
	DEL_CLASS(listener);
	hdlr.Delete();
	return 0;
}