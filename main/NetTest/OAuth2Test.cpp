#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Net/OSSocketFactory.h"
#include "Net/WebServer/OAuth2Handler.h"
#include "Net/WebServer/PrintLogWebHandler.h"
#include "Net/WebServer/WebListener.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	IO::ConsoleWriter console;
	Net::SocketFactory *sockf;
	Net::WebServer::OAuth2Handler *oauth2Hdlr;
	Net::WebServer::PrintLogWebHandler *hdlr;
	Net::WebServer::WebListener *listener;
	NEW_CLASS(sockf, Net::OSSocketFactory(false));
	NEW_CLASS(oauth2Hdlr, Net::WebServer::OAuth2Handler((const UTF8Char*)"/auth", (const UTF8Char*)"/token", (const UTF8Char*)"/userinfo"));
	NEW_CLASS(hdlr, Net::WebServer::PrintLogWebHandler(oauth2Hdlr, &console));
	NEW_CLASS(listener, Net::WebServer::WebListener(sockf, 0, hdlr, 8889, 30, 4, (const UTF8Char*)"OAuthTest/1.0", false, true));
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
	hdlr->Release();
	DEL_CLASS(sockf);
	return 0;
}