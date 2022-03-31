#include "Stdafx.h"
#include "Net/WebServer/OAuth2Handler.h"

Bool __stdcall Net::WebServer::OAuth2Handler::AuthHandler(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, WebServiceHandler *me)
{
	return false;
}

Bool __stdcall Net::WebServer::OAuth2Handler::TokenHandler(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, WebServiceHandler *me)
{
	return false;
}

Bool __stdcall Net::WebServer::OAuth2Handler::UserInfoHandler(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, WebServiceHandler *me)
{
	return false;
}

Net::WebServer::OAuth2Handler::OAuth2Handler(Text::CString authPath, Text::CString tokenPath, Text::CString userinfoPath)
{
	this->AddService(authPath, Net::WebUtil::RequestMethod::HTTP_POST, AuthHandler);
	this->AddService(tokenPath, Net::WebUtil::RequestMethod::HTTP_POST, TokenHandler);
	this->AddService(userinfoPath, Net::WebUtil::RequestMethod::HTTP_POST, UserInfoHandler);
}

Net::WebServer::OAuth2Handler::~OAuth2Handler()
{
}
