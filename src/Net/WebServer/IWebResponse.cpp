#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/DateTime.h"
#include "Net/WebStatus.h"
#include "Net/WebServer/IWebResponse.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/TextBinEnc/URIEncoding.h"

Net::WebServer::IWebResponse::IWebResponse(Text::String *sourceName) : IO::Stream(sourceName)
{
}

Net::WebServer::IWebResponse::IWebResponse(Text::CString sourceName) : IO::Stream(sourceName)
{
}

Net::WebServer::IWebResponse::~IWebResponse()
{
}

Bool Net::WebServer::IWebResponse::ResponseError(Net::WebServer::IWebRequest *req, Net::WebStatus::StatusCode code)
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
	Text::CString codeName = Net::WebStatus::GetCodeName(code);
	sb.AppendNE(codeName.v, codeName.leng);
	sb.AppendNE(UTF8STRC("</title>\r\n"));
	sb.AppendNE(UTF8STRC("</head><body>\r\n"));
	sb.AppendNE(UTF8STRC("<h1>"));
	sb.AppendNE(codeName.v, codeName.leng);
	sb.AppendNE(UTF8STRC("</h1>\r\n"));
	sb.AppendC(UTF8STRC("</body></html>\r\n"));
	this->AddContentLength(sb.GetLength());
	this->Write(sb.ToString(), sb.GetLength());
	return true;
}

Bool Net::WebServer::IWebResponse::RedirectURL(Net::WebServer::IWebRequest *req, Text::CString url, OSInt cacheAge)
{
	this->AddDefHeaders(req);
	this->SetStatusCode(Net::WebStatus::SC_MOVED_TEMPORARILY);
	this->AddCacheControl(cacheAge);
	this->AddHeader(CSTR("Location"), url);
	this->AddContentLength(0);
	return true;
}

Bool Net::WebServer::IWebResponse::ResponseNotModified(Net::WebServer::IWebRequest *req, OSInt cacheAge)
{
	this->SetStatusCode(Net::WebStatus::SC_NOT_MODIFIED);
	this->AddDefHeaders(req);
	this->AddCacheControl(cacheAge);
	this->AddContentLength(0);
	return true;
}

Bool Net::WebServer::IWebResponse::ResponseText(Text::CString txt)
{
	return ResponseText(txt, CSTR_NULL);
}

Bool Net::WebServer::IWebResponse::ResponseText(Text::CString txt, Text::CString contentType)
{
	this->AddContentLength(txt.leng);
	if (contentType.v == 0)
	{
		this->AddContentType(CSTR("text/plain"));
	}
	else
	{
		this->AddContentType(contentType);
	}
	this->Write(txt.v, txt.leng);
	return true;
}

Bool Net::WebServer::IWebResponse::ResponseJSONStr(Net::WebServer::IWebRequest *req, OSInt cacheAge, Text::CString json)
{
	this->AddDefHeaders(req);
	this->AddCacheControl(cacheAge);
	return this->ResponseText(json, CSTR("application/json"));
}

UTF8Char *Net::WebServer::IWebResponse::ToTimeString(UTF8Char *buff, Data::DateTime *dt)
{
	static const Char *wdays[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
	dt->ToUTCTime();
	return Text::StrConcatC(dt->ToString(Text::StrConcatC(buff, (const UTF8Char*)wdays[(OSInt)dt->GetWeekday()], 3), ", dd MMM yyyy HH:mm:ss"), UTF8STRC(" GMT"));
}

UTF8Char *Net::WebServer::IWebResponse::ToTimeString(UTF8Char *buff, const Data::Timestamp &ts)
{
	static const Char *wdays[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
	Data::DateTime dt(ts.ToTicks(), 0);
	return Text::StrConcatC(dt.ToString(Text::StrConcatC(buff, (const UTF8Char*)wdays[(OSInt)dt.GetWeekday()], 3), ", dd MMM yyyy HH:mm:ss"), UTF8STRC(" GMT"));
}
