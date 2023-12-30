#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Net/OSSocketFactory.h"
#include "Net/WebServer/OAuth2Handler.h"
#include "Net/WebServer/PrintLogWebHandler.h"
#include "Net/WebServer/WebListener.h"

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	IO::ConsoleWriter console;
	NotNullPtr<Net::WebServer::OAuth2Handler> oauth2Hdlr;
	NotNullPtr<Net::WebServer::PrintLogWebHandler> hdlr;
	Net::WebServer::WebListener *listener;
	Net::OSSocketFactory sockf(false);
	NEW_CLASSNN(oauth2Hdlr, Net::WebServer::OAuth2Handler(CSTR("/auth"), CSTR("/token"), CSTR("/userinfo")));
	NEW_CLASSNN(hdlr, Net::WebServer::PrintLogWebHandler(oauth2Hdlr, console));
	NEW_CLASS(listener, Net::WebServer::WebListener(sockf, 0, hdlr, 8889, 30, 1, 4, CSTR("OAuthTest/1.0"), false, Net::WebServer::KeepAlive::Default, true));
	if (!listener->IsError())
	{
		console.WriteLineC(UTF8STRC("OAuth2 Test running at port 8889"));
		progCtrl->WaitForExit(progCtrl);
		console.WriteLineC(UTF8STRC("OAuth2 Test is stopping"));
	}
	else
	{
		console.WriteLineC(UTF8STRC("Error in listening to port 8889"));
	}
	DEL_CLASS(listener);
	hdlr.Delete();
	return 0;
}