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

Net::WebServer::IWebResponse::IWebResponse(const UTF8Char *sourceName, UOSInt nameLen) : IO::Stream(sourceName, nameLen)
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
	this->AddContentType(UTF8STRC("text/html"));
	sb.AppendC(UTF8STRC("<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n"));
	sb.AppendC(UTF8STRC("<html><head>\r\n"));
	sb.AppendC(UTF8STRC("<title>"));
	sb.AppendI32(code);
	sb.AppendC(UTF8STRC(" "));
	Text::CString codeName = Net::WebStatus::GetCodeName(code);
	sb.AppendC(codeName.v, codeName.leng);
	sb.AppendC(UTF8STRC("</title>\r\n"));
	sb.AppendC(UTF8STRC("</head><body>\r\n"));
	sb.AppendC(UTF8STRC("<h1>"));
	sb.AppendC(codeName.v, codeName.leng);
	sb.AppendC(UTF8STRC("</h1>\r\n"));
	sb.AppendC(UTF8STRC("</body></html>\r\n"));
	this->AddContentLength(sb.GetLength());
	this->Write(sb.ToString(), sb.GetLength());
	return true;
}

Bool Net::WebServer::IWebResponse::RedirectURL(Net::WebServer::IWebRequest *req, const UTF8Char *url, UOSInt urlLen, OSInt cacheAge)
{
	this->AddDefHeaders(req);
	this->SetStatusCode(Net::WebStatus::SC_MOVED_TEMPORARILY);
	this->AddCacheControl(cacheAge);
	this->AddHeaderC(UTF8STRC("Location"), url, urlLen);
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

Bool Net::WebServer::IWebResponse::ResponseText(const UTF8Char *txt)
{
	return ResponseText(txt, 0);
}

Bool Net::WebServer::IWebResponse::ResponseText(const UTF8Char *txt, const UTF8Char *contentType)
{
	UOSInt len = Text::StrCharCnt(txt);
	this->AddContentLength(len);
	if (contentType == 0)
	{
		this->AddContentType(UTF8STRC("text/plain"));
	}
	else
	{
		this->AddContentType(contentType, Text::StrCharCnt(contentType));
	}
	this->Write(txt, len);
	return true;
}

Bool Net::WebServer::IWebResponse::AddHeaderS(const UTF8Char *name, UOSInt nameLen, Text::String *value)
{
	return AddHeaderC(name, nameLen, value->v, value->leng);
}

Bool Net::WebServer::IWebResponse::AddCacheControl(OSInt cacheAge)
{
	if (cacheAge == -2)
	{

	}
	else if (cacheAge == -1)
	{
		return this->AddHeaderC(UTF8STRC("Cache-Control"), UTF8STRC("private"));
	}
	else if (cacheAge == 0)
	{
		return this->AddHeaderC(UTF8STRC("Cache-Control"), UTF8STRC("no-cache"));
	}
	else
	{
		UTF8Char sbuff[256];
		UTF8Char *sptr;
		sptr = Text::StrOSInt(Text::StrConcatC(sbuff, UTF8STRC("private; max-age=")), cacheAge);
		return this->AddHeaderC(UTF8STRC("Cache-Control"), sbuff, (UOSInt)(sptr - sbuff));
	}
	return true;
}

Bool Net::WebServer::IWebResponse::AddTimeHeader(const UTF8Char *name, UOSInt nameLen, Data::DateTime *dt)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr = ToTimeString(sbuff, dt);
	return this->AddHeaderC(name, nameLen, sbuff, (UOSInt)(sptr - sbuff));
}

Bool Net::WebServer::IWebResponse::AddContentDisposition(Bool isAttachment, const UTF8Char *attFileName, Net::BrowserInfo::BrowserType browser)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	if (isAttachment)
	{
		sptr = Text::StrConcatC(sbuff, UTF8STRC("attachment"));
	}
	else
	{
		sptr = Text::StrConcatC(sbuff, UTF8STRC("inline"));
	}
	if (attFileName)
	{
		sptr = Text::StrConcatC(sptr, UTF8STRC("; filename=\""));
		if (browser == Net::BrowserInfo::BT_IE)
		{
			sptr = Text::TextBinEnc::URIEncoding::URIEncode(sptr, attFileName);
		}
		else
		{
			sptr = Text::TextBinEnc::URIEncoding::URIEncode(sptr, attFileName);
		}
		sptr = Text::StrConcatC(sptr, UTF8STRC("\""));
	}
	return this->AddHeaderC(UTF8STRC("Content-Disposition"), sbuff, (UOSInt)(sptr - sbuff));
}

Bool Net::WebServer::IWebResponse::AddContentLength(UInt64 contentLeng)
{
	UTF8Char sbuff[22];
	UTF8Char *sptr = Text::StrUInt64(sbuff, contentLeng);
	return this->AddHeaderC(UTF8STRC("Content-Length"), sbuff, (UOSInt)(sptr - sbuff));
}

Bool Net::WebServer::IWebResponse::AddContentType(const UTF8Char *contentType, UOSInt len)
{
	return this->AddHeaderC(UTF8STRC("Content-Type"), contentType, len);
}

Bool Net::WebServer::IWebResponse::AddDate(Data::DateTime *dt)
{
	return this->AddTimeHeader(UTF8STRC("Date"), dt);
}

Bool Net::WebServer::IWebResponse::AddExpireTime(Data::DateTime *dt)
{
	if (dt == 0)
	{
		return this->AddHeaderC(UTF8STRC("Expires"), UTF8STRC("0"));
	}
	else
	{
		return this->AddTimeHeader(UTF8STRC("Expires"), dt);
	}
}

Bool Net::WebServer::IWebResponse::AddLastModified(Data::DateTime *dt)
{
	return this->AddTimeHeader(UTF8STRC("Last-Modified"), dt);
}

Bool Net::WebServer::IWebResponse::AddServer(const UTF8Char *server, UOSInt len)
{
	return this->AddHeaderC(UTF8STRC("Server"), server, len);
}

UTF8Char *Net::WebServer::IWebResponse::ToTimeString(UTF8Char *buff, Data::DateTime *dt)
{
	static const Char *wdays[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
	dt->ToUTCTime();
	return Text::StrConcatC(dt->ToString(Text::StrConcatC(buff, (const UTF8Char*)wdays[(OSInt)dt->GetWeekday()], 3), ", dd MMM yyyy HH:mm:ss"), UTF8STRC(" GMT"));
}
