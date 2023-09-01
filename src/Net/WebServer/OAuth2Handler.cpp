#include "Stdafx.h"
#include "Net/WebServer/OAuth2Handler.h"

Bool __stdcall Net::WebServer::OAuth2Handler::AuthHandler(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, WebServiceHandler *me)
{
	return false;
}

Bool __stdcall Net::WebServer::OAuth2Handler::TokenHandler(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, WebServiceHandler *me)
{
	return false;
}

Bool __stdcall Net::WebServer::OAuth2Handler::UserInfoHandler(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, WebServiceHandler *me)
{
	return false;
}

Net::WebServer::OAuth2Handler::OAuth2Handler(Text::CStringNN authPath, Text::CStringNN tokenPath, Text::CStringNN userinfoPath)
{
	this->AddService(authPath, Net::WebUtil::RequestMethod::HTTP_POST, AuthHandler);
	this->AddService(tokenPath, Net::WebUtil::RequestMethod::HTTP_POST, TokenHandler);
	this->AddService(userinfoPath, Net::WebUtil::RequestMethod::HTTP_POST, UserInfoHandler);
}

Net::WebServer::OAuth2Handler::~OAuth2Handler()
{
}
