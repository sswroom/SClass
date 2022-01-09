#include "Stdafx.h"
#include "Net/WebServer/OAuth2Handler.h"

Bool __stdcall Net::WebServer::OAuth2Handler::AuthHandler(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, WebServiceHandler *me)
{
	return false;
}

Bool __stdcall Net::WebServer::OAuth2Handler::TokenHandler(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, WebServiceHandler *me)
{
	return false;
}

Bool __stdcall Net::WebServer::OAuth2Handler::UserInfoHandler(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, WebServiceHandler *me)
{
	return false;
}

Net::WebServer::OAuth2Handler::OAuth2Handler(const UTF8Char *authPath, const UTF8Char *tokenPath, const UTF8Char *userinfoPath)
{
	this->AddService(authPath, Text::StrCharCnt(authPath), Net::WebServer::IWebRequest::RequestMethod::HTTP_POST, AuthHandler);
	this->AddService(tokenPath, Text::StrCharCnt(tokenPath), Net::WebServer::IWebRequest::RequestMethod::HTTP_POST, TokenHandler);
	this->AddService(userinfoPath, Text::StrCharCnt(userinfoPath), Net::WebServer::IWebRequest::RequestMethod::HTTP_POST, UserInfoHandler);
}

Net::WebServer::OAuth2Handler::~OAuth2Handler()
{
}
