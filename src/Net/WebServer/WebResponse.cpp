#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/DateTime.h"
#include "Net/WebStatus.h"
#include "Net/WebServer/WebResponse.h"
#include "Text/JSText.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/TextBinEnc/URIEncoding.h"

Net::WebServer::WebResponse::WebResponse(NN<Text::String> sourceName) : IO::Stream(sourceName)
{
}

Net::WebServer::WebResponse::WebResponse(Text::CStringNN sourceName) : IO::Stream(sourceName)
{
}

Net::WebServer::WebResponse::~WebResponse()
{
}

Bool Net::WebServer::WebResponse::ResponseError(NN<Net::WebServer::WebRequest> req, Net::WebStatus::StatusCode code)
{
	Text::StringBuilderUTF8 sb;
	if (!this->SetStatusCode(code))
		return false;
	this->AddDefHeaders(req);
	this->AddContentType(CSTR("text/html"));
	sb.AppendNE(UTF8STRC("<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n"));
	sb.AppendNE(UTF8STRC("<html><head>\r\n"));
	sb.AppendNE(UTF8STRC("<title>"));
	sb.AppendI32(code);
	sb.AppendNE(UTF8STRC(" "));
	Text::CStringNN codeName = Net::WebStatus::GetCodeName(code).Or(CSTR("Unknown"));
	sb.AppendNE(codeName.v, codeName.leng);
	sb.AppendNE(UTF8STRC("</title>\r\n"));
	sb.AppendNE(UTF8STRC("</head><body>\r\n"));
	sb.AppendNE(UTF8STRC("<h1>"));
	sb.AppendNE(codeName.v, codeName.leng);
	sb.AppendNE(UTF8STRC("</h1>\r\n"));
	sb.AppendC(UTF8STRC("</body></html>\r\n"));
	this->AddContentLength(sb.GetLength());
	this->Write(sb.ToByteArray());
	return true;
}

Bool Net::WebServer::WebResponse::RedirectURL(NN<Net::WebServer::WebRequest> req, Text::CStringNN url, OSInt cacheAge)
{
	this->AddDefHeaders(req);
	this->SetStatusCode(Net::WebStatus::SC_MOVED_TEMPORARILY);
	this->AddCacheControl(cacheAge);
	this->AddHeader(CSTR("Location"), url);
	this->AddContentLength(0);
	return true;
}

Bool Net::WebServer::WebResponse::VirtualRedirectURL(NN<Net::WebServer::WebRequest> req, Text::CStringNN url, OSInt cacheAge)
{
	this->AddDefHeaders(req);
	this->AddCacheControl(cacheAge);
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("<html><head><title>Redirecting</title>\r\n"
	"<script type=\"text/javascript\">\r\n"
	"function afterLoad(){\r\n"
	"	document.location.replace("));
	NN<Text::String> s = Text::JSText::ToNewJSText(url.v);
	sb.Append(s);
	s->Release();
	sb.AppendC(UTF8STRC(");\r\n"
	"}\r\n"
	"</script>\r\n"
	"</head><body onLoad=\"afterLoad()\"></body></html>"));
	this->AddContentLength(sb.leng);
	this->Write(sb.ToByteArray());
	return true;
}

Bool Net::WebServer::WebResponse::ResponseNotModified(NN<Net::WebServer::WebRequest> req, OSInt cacheAge)
{
	this->SetStatusCode(Net::WebStatus::SC_NOT_MODIFIED);
	this->AddDefHeaders(req);
	this->AddCacheControl(cacheAge);
	this->AddContentLength(0);
	return true;
}

Bool Net::WebServer::WebResponse::ResponseText(Text::CStringNN txt)
{
	return ResponseText(txt, CSTR("text/plain"));
}

Bool Net::WebServer::WebResponse::ResponseText(Text::CStringNN txt, Text::CStringNN contentType)
{
	this->AddContentLength(txt.leng);
	this->AddContentType(contentType);
	this->Write(txt.ToByteArray());
	return true;
}

