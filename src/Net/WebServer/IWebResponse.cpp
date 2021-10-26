#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/DateTime.h"
#include "Net/WebStatus.h"
#include "Net/WebServer/IWebResponse.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/TextEnc/URIEncoding.h"

Net::WebServer::IWebResponse::IWebResponse(const UTF8Char *sourceName) : IO::Stream(sourceName)
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
	this->AddContentType((const UTF8Char*)"text/html");
	sb.Append((const UTF8Char*)"<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n");
	sb.Append((const UTF8Char*)"<html><head>\r\n");
	sb.Append((const UTF8Char*)"<title>");
	sb.AppendI32(code);
	sb.Append((const UTF8Char*)" ");
	sb.Append(Net::WebStatus::GetCodeName(code));
	sb.Append((const UTF8Char*)"</title>\r\n");
	sb.Append((const UTF8Char*)"</head><body>\r\n");
	sb.Append((const UTF8Char*)"<h1>");
	sb.Append(Net::WebStatus::GetCodeName(code));
	sb.Append((const UTF8Char*)"</h1>\r\n");
	sb.Append((const UTF8Char*)"</body></html>\r\n");
	this->AddContentLength(sb.GetLength());
	this->Write(sb.ToString(), sb.GetLength());
	return true;
}

Bool Net::WebServer::IWebResponse::RedirectURL(Net::WebServer::IWebRequest *req, const UTF8Char *url, OSInt cacheAge)
{
	this->AddDefHeaders(req);
	this->SetStatusCode(Net::WebStatus::SC_MOVED_TEMPORARILY);
	this->AddCacheControl(cacheAge);
	this->AddHeader((const UTF8Char*)"Location", url);
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
		this->AddContentType((const UTF8Char*)"text/plain");
	}
	else
	{
		this->AddContentType(contentType);
	}
	this->Write(txt, len);
	return true;
}

Bool Net::WebServer::IWebResponse::AddCacheControl(OSInt cacheAge)
{
	if (cacheAge == -2)
	{

	}
	else if (cacheAge == -1)
	{
		return this->AddHeader((const UTF8Char*)"Cache-Control", (const UTF8Char*)"private");
	}
	else if (cacheAge == 0)
	{
		return this->AddHeader((const UTF8Char*)"Cache-Control", (const UTF8Char*)"no-cache");
	}
	else
	{
		UTF8Char sbuff[256];
		Text::StrOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"private; max-age="), cacheAge);
		return this->AddHeader((const UTF8Char*)"Cache-Control", sbuff);
	}
	return true;
}

Bool Net::WebServer::IWebResponse::AddTimeHeader(const UTF8Char *name, Data::DateTime *dt)
{
	UTF8Char sbuff[256];
	ToTimeString(sbuff, dt);
	return this->AddHeader(name, sbuff);
}

Bool Net::WebServer::IWebResponse::AddContentDisposition(Bool isAttachment, const UTF8Char *attFileName, Net::BrowserInfo::BrowserType browser)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	if (isAttachment)
	{
		sptr = Text::StrConcat(sbuff, (const UTF8Char*)"attachment");
	}
	else
	{
		sptr = Text::StrConcat(sbuff, (const UTF8Char*)"inline");
	}
	if (attFileName)
	{
		sptr = Text::StrConcat(sptr, (const UTF8Char*)"; filename=\"");
		if (browser == Net::BrowserInfo::BT_IE)
		{
			sptr = Text::TextEnc::URIEncoding::URIEncode(sptr, attFileName);
		}
		else
		{
			sptr = Text::TextEnc::URIEncoding::URIEncode(sptr, attFileName);
		}
		sptr = Text::StrConcat(sptr, (const UTF8Char*)"\"");
	}
	return this->AddHeader((const UTF8Char*)"Content-Disposition", sbuff);
}

Bool Net::WebServer::IWebResponse::AddContentLength(UInt64 contentLeng)
{
	UTF8Char sbuff[22];
	Text::StrUInt64(sbuff, contentLeng);
	return this->AddHeader((const UTF8Char*)"Content-Length", sbuff);
}

Bool Net::WebServer::IWebResponse::AddContentType(const UTF8Char *contentType)
{
	return this->AddHeader((const UTF8Char*)"Content-Type", contentType);
}

Bool Net::WebServer::IWebResponse::AddDate(Data::DateTime *dt)
{
	return this->AddTimeHeader((const UTF8Char*)"Date", dt);
}

Bool Net::WebServer::IWebResponse::AddExpireTime(Data::DateTime *dt)
{
	if (dt == 0)
	{
		return this->AddHeader((const UTF8Char*)"Expires", (const UTF8Char*)"0");
	}
	else
	{
		return this->AddTimeHeader((const UTF8Char*)"Expires", dt);
	}
}

Bool Net::WebServer::IWebResponse::AddLastModified(Data::DateTime *dt)
{
	return this->AddTimeHeader((const UTF8Char*)"Last-Modified", dt);
}

Bool Net::WebServer::IWebResponse::AddServer(const UTF8Char *server)
{
	return this->AddHeader((const UTF8Char*)"Server", server);
}

UTF8Char *Net::WebServer::IWebResponse::ToTimeString(UTF8Char *buff, Data::DateTime *dt)
{
	static const Char *wdays[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
	dt->ToUTCTime();
	return Text::StrConcat(dt->ToString(Text::StrConcat(buff, (const UTF8Char*)wdays[dt->GetWeekday()]), ", dd MMM yyyy HH:mm:ss"), (const UTF8Char*)" GMT");
}
