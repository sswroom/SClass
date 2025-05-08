#include "Stdafx.h"
#include "Net/WebServer/SAMLService.h"

Bool __stdcall Net::WebServer::SAMLService::GetLoginFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr)
{
	NN<SAMLService> me = NN<SAMLService>::ConvertFrom(svcHdlr);
	if (!me->hdlr->DoLoginGet(req, resp))
	{
		resp->ResponseError(req, Net::WebStatus::SC_INTERNAL_SERVER_ERROR);
	}
	return true;
}

Bool __stdcall Net::WebServer::SAMLService::GetLogoutFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr)
{
	NN<SAMLService> me = NN<SAMLService>::ConvertFrom(svcHdlr);
	if (!me->hdlr->DoLogoutGet(req, resp, CSTR("id")))
	{
		resp->ResponseError(req, Net::WebStatus::SC_INTERNAL_SERVER_ERROR);
	}
	return true;
}

Bool __stdcall Net::WebServer::SAMLService::PostSSOFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr)
{
	return false;
}

Bool __stdcall Net::WebServer::SAMLService::GetMetadataFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr)
{
	NN<SAMLService> me = NN<SAMLService>::ConvertFrom(svcHdlr);
	if (!me->hdlr->DoMetadataGet(req, resp))
	{
		resp->ResponseError(req, Net::WebStatus::SC_INTERNAL_SERVER_ERROR);
	}
	return true;
}

Net::WebServer::SAMLService::SAMLService(NN<SAMLHandler> hdlr)
{
	this->hdlr = hdlr;
	this->rawRespHdlr = 0;
	this->rawRespObj = 0;
	this->loginHdlr = 0;
	this->loginObj = 0;

	this->AddService(Text::String::OrEmpty(hdlr->GetLoginPath())->ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, GetLoginFunc);
	this->AddService(Text::String::OrEmpty(hdlr->GetLogoutPath())->ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, GetLogoutFunc);
	this->AddService(Text::String::OrEmpty(hdlr->GetMetadataPath())->ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, GetMetadataFunc);
	this->AddService(Text::String::OrEmpty(hdlr->GetSSOPath())->ToCString(), Net::WebUtil::RequestMethod::HTTP_POST, PostSSOFunc);
}

Net::WebServer::SAMLService::~SAMLService()
{
	this->hdlr.Delete();
}

void Net::WebServer::SAMLService::HandleRAWSAMLResponse(SAMLStrFunc hdlr, AnyType userObj)
{
	this->rawRespHdlr = hdlr;
	this->rawRespObj = userObj;
}

void Net::WebServer::SAMLService::HandleLoginRequest(SAMLLoginFunc hdlr, AnyType userObj)
{
	this->loginHdlr = hdlr;
	this->loginObj = userObj;
}

