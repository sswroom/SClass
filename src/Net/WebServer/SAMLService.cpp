#include "Stdafx.h"
#include "IO/MemoryReadingStream.h"
#include "Net/WebServer/HTTPServerUtil.h"
#include "Net/WebServer/SAMLService.h"
#include "Text/XML.h"
#include "Text/XMLReader.h"

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
	Text::CString nameID = 0;
	Text::CString sessionIndex = 0;
	NN<Text::String> s;
	if (req->GetQueryValue(CSTR("nameID")).SetTo(s))
	{
		nameID = s->ToCString();
	}
	if (req->GetQueryValue(CSTR("sessionIndex")).SetTo(s))
	{
		sessionIndex = s->ToCString();
	}
	if (!me->hdlr->DoLogoutGet(req, resp, nameID, sessionIndex))
	{
		resp->ResponseError(req, Net::WebStatus::SC_INTERNAL_SERVER_ERROR);
	}
	return true;
}

Bool __stdcall Net::WebServer::SAMLService::PostSSOFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr)
{
	NN<SAMLService> me = NN<SAMLService>::ConvertFrom(svcHdlr);
	NN<Net::SAMLSSOResponse> res = me->hdlr->DoSSOPost(req, resp);
	NN<Text::String> s;
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb2;
	sb.ClearStr();
	sb.AppendC(UTF8STRC("<html><head><title>SSO Message</title></head><body>"));
	sb.AppendC(UTF8STRC("<h1>Result</h1>"));
	sb.AppendC(UTF8STRC("<font color=\"red\">Error:</font> "));
	sb.Append(Net::SAMLSSOResponse::ResponseErrorGetName(res->GetError()));
	sb.Append(CSTR("<br/>"));
	sb.AppendC(UTF8STRC("<font color=\"red\">Error Message:</font> "));
	sb.Append(res->GetErrorMessage());
	sb.Append(CSTR("<br/>"));
	sb.AppendC(UTF8STRC("<font color=\"red\">Id:</font> "));
	sb.AppendOpt(res->GetId());
	sb.Append(CSTR("<br/>"));
	sb.AppendC(UTF8STRC("<font color=\"red\">SessionIndex:</font> "));
	sb.AppendOpt(res->GetSessionIndex());
	sb.Append(CSTR("<br/>"));
	sb.Append(CSTR("<font color=\"red\">NameID:</font> "));
	sb.AppendOpt(res->GetNameID());
	sb.Append(CSTR("<br/>"));
	sb.Append(CSTR("<font color=\"red\">Name:</font> "));
	sb.AppendOpt(res->GetName());
	sb.Append(CSTR("<br/>"));
	sb.Append(CSTR("<font color=\"red\">GivenName:</font> "));
	sb.AppendOpt(res->GetGivenname());
	sb.Append(CSTR("<br/>"));
	sb.Append(CSTR("<font color=\"red\">Surname:</font> "));
	sb.AppendOpt(res->GetSurname());
	sb.Append(CSTR("<br/>"));
	sb.Append(CSTR("<font color=\"red\">EmailAddress:</font> "));
	sb.AppendOpt(res->GetEmailAddress());
	sb.Append(CSTR("<br/>"));
	sb.Append(CSTR("<font color=\"red\">Group:</font> "));
	s = res->GetGroup()->JoinString(CSTR(", "));
	sb.Append(s);
	s->Release();
	sb.Append(CSTR("<br/>"));

	if (res->GetDecResponse().SetTo(s))
	{
		sb.AppendC(UTF8STRC("<hr/>"));
		sb.AppendC(UTF8STRC("<h1>Decrypted Content</h1>"));
		sb2.ClearStr();
		IO::MemoryReadingStream mstm(s->ToByteArray());
		Text::XMLReader::XMLWellFormat(me->encFact, mstm, 0, sb2);
		s = Text::XML::ToNewHTMLTextXMLColor(sb2.v);
		sb.Append(s);
		s->Release();
	}
	if (res->GetRawResponse().SetTo(s))
	{
		sb.AppendC(UTF8STRC("<hr/>"));
		sb.AppendC(UTF8STRC("<h1>RAW Response</h1>"));
		sb2.ClearStr();
		IO::MemoryReadingStream mstm(s->ToByteArray());
		Text::XMLReader::XMLWellFormat(me->encFact, mstm, 0, sb2);
		s = Text::XML::ToNewHTMLTextXMLColor(sb2.v);
		sb.Append(s);
		s->Release();
	}
	sb.AppendC(UTF8STRC("</body></html>"));
	res.Delete();
	resp->AddDefHeaders(req);
	resp->AddCacheControl(0);
	resp->AddContentType(CSTR("text/html"));
	Net::WebServer::HTTPServerUtil::SendContent(req, resp, CSTR("text/html"), sb.ToCString());

	return true;
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
	this->respHdlr = 0;
	this->respObj = 0;

	this->AddService(Text::String::OrEmpty(hdlr->GetLoginPath())->ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, GetLoginFunc);
	this->AddService(Text::String::OrEmpty(hdlr->GetLogoutPath())->ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, GetLogoutFunc);
	this->AddService(Text::String::OrEmpty(hdlr->GetLogoutPath())->ToCString(), Net::WebUtil::RequestMethod::HTTP_POST, GetLogoutFunc);
	this->AddService(Text::String::OrEmpty(hdlr->GetMetadataPath())->ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, GetMetadataFunc);
	this->AddService(Text::String::OrEmpty(hdlr->GetSSOPath())->ToCString(), Net::WebUtil::RequestMethod::HTTP_POST, PostSSOFunc);
}

Net::WebServer::SAMLService::~SAMLService()
{
	this->hdlr.Delete();
}

void Net::WebServer::SAMLService::HandleSAMLResponse(SAMLRespFunc hdlr, AnyType userObj)
{
	this->respHdlr = hdlr;
	this->respObj = userObj;
}
