#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/UserAgentDB.h"
#include "Net/WebServer/IWebRequest.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/URLString.h"

UTF8Char Net::WebServer::IWebRequest::PARAM_SEPERATOR = '|';

void Net::WebServer::IWebRequest::ParseUserAgent()
{
	this->uaParsed = true;
	this->reqBrowser = Net::BrowserInfo::BT_UNKNOWN;

	Text::String *uaHdr = this->GetSHeader(UTF8STRC("User-Agent"));
	if (uaHdr == 0)
	{
		return;
	}
	const Net::UserAgentDB::UAEntry *ent = Net::UserAgentDB::GetUserAgentInfo(uaHdr->v);
	if (ent)
	{
		this->reqBrowser = ent->browser;
		if (ent->browserVer)
		{
			this->reqBrowserVer = Text::StrCopyNew((const UTF8Char*)ent->browserVer);
		}
		else
		{
			this->reqBrowserVer = 0;
		}
		this->reqOS = ent->os;
		if (ent->osVer)
		{
			this->reqOSVer = Text::StrCopyNew((const UTF8Char*)ent->osVer);
		}
		else
		{
			this->reqOSVer = 0;
		}
		return;
	}
	Net::UserAgentDB::UAEntry ua;
	Net::UserAgentDB::ParseUserAgent(&ua, uaHdr->v);
	this->reqBrowser = ua.browser;
	this->reqBrowserVer = (const UTF8Char*)ua.browserVer;
	this->reqOS = ua.os;
	this->reqOSVer = (const UTF8Char*)ua.osVer;
	SDEL_TEXT(ua.devName);
}

Net::WebServer::IWebRequest::IWebRequest()
{
	this->uaParsed = false;
	this->reqBrowser = Net::BrowserInfo::BT_UNKNOWN;
	this->reqBrowserVer = 0;
	this->reqOS = Manage::OSInfo::OT_UNKNOWN;
	this->reqOSVer = 0;
}

Net::WebServer::IWebRequest::~IWebRequest()
{
	SDEL_TEXT(this->reqBrowserVer);
	SDEL_TEXT(this->reqOSVer);
}

Bool Net::WebServer::IWebRequest::GetRefererDomain(Text::StringBuilderUTF8 *sb)
{
	Text::String *hdr;
	UTF8Char domain[256];
	UTF8Char *sptr;
	hdr = this->GetSHeader(UTF8STRC("Referer"));
	if (hdr == 0)
	{
		return false;
	}
	sptr = Text::URLString::GetURLDomain(domain, hdr->v, hdr->leng, 0);
	if (sptr == 0)
	{
		return false;
	}
	else
	{
		sb->AppendC(domain, (UOSInt)(sptr - domain));
		return true;
	}
}

Bool Net::WebServer::IWebRequest::GetIfModifiedSince(Data::DateTime *dt)
{
	Text::String *s = this->GetSHeader(UTF8STRC("If-Modified-Since"));
	if (s)
	{
		if (dt->SetValue(s->v, s->leng))
		{
			return true;
		}
	}
	return false;
}

UTF8Char *Net::WebServer::IWebRequest::GetRequestPath(UTF8Char *sbuff, UOSInt maxLeng)
{
	const UTF8Char *uri = this->GetRequestURI()->v;
	UTF8Char c;
	while ((c = *uri++) != 0)
	{
		if (c == '?' || c == 0 || c == '#')
		{
			break;
		}
		if (maxLeng-- <= 0)
			break;
		*sbuff++ = c;
	}
	*sbuff = 0;
	return sbuff;
}

UTF8Char *Net::WebServer::IWebRequest::GetQueryString(UTF8Char *sbuff, UOSInt maxLeng)
{
	Text::String *s = this->GetRequestURI();
	UOSInt i = s->IndexOf('?');
	if (i == INVALID_INDEX)
		return 0;
	const UTF8Char *uri = &s->v[i + 1];

	UTF8Char c;
	while ((c = *uri++) != 0)
	{
		if (c == 0)
		{
			break;
		}
		if (maxLeng-- <= 0)
			break;
		*sbuff++ = c;
	}
	*sbuff = 0;
	return sbuff;
	
}

UTF8Char *Net::WebServer::IWebRequest::GetQueryValueStr(const UTF8Char *name, UOSInt nameLen, UTF8Char *buff, UOSInt buffSize)
{
	Text::String *s = this->GetQueryValue(name, nameLen);
	if (s == 0)
		return 0;
	return Text::StrConcatS(buff, s->v, buffSize);
}

Bool Net::WebServer::IWebRequest::GetQueryValueI16(const UTF8Char *name, UOSInt nameLen, Int16 *val)
{
	Text::String *s = this->GetQueryValue(name, nameLen);
	if (s == 0)
		return false;
	return s->ToInt16(val);
}

Bool Net::WebServer::IWebRequest::GetQueryValueU16(const UTF8Char *name, UOSInt nameLen, UInt16 *val)
{
	Text::String *s = this->GetQueryValue(name, nameLen);
	if (s == 0)
		return false;
	return s->ToUInt16(val);
}

Bool Net::WebServer::IWebRequest::GetQueryValueI32(const UTF8Char *name, UOSInt nameLen, Int32 *val)
{
	Text::String *s = this->GetQueryValue(name, nameLen);
	if (s == 0)
		return false;
	return s->ToInt32(val);
}

Bool Net::WebServer::IWebRequest::GetQueryValueU32(const UTF8Char *name, UOSInt nameLen, UInt32 *val)
{
	Text::String *s = this->GetQueryValue(name, nameLen);
	if (s == 0)
		return false;
	return s->ToUInt32(val);
}

Bool Net::WebServer::IWebRequest::GetQueryValueI64(const UTF8Char *name, UOSInt nameLen, Int64 *val)
{
	Text::String *s = this->GetQueryValue(name, nameLen);
	if (s == 0)
		return false;
	return s->ToInt64(val);
}

Bool Net::WebServer::IWebRequest::GetQueryValueF64(const UTF8Char *name, UOSInt nameLen, Double *val)
{
	Text::String *s = this->GetQueryValue(name, nameLen);
	if (s == 0)
		return false;
	return s->ToDouble(val);
}

Bool Net::WebServer::IWebRequest::GetHTTPFormInt16(const UTF8Char *name, UOSInt nameLen, Int16 *valOut)
{
	Text::String *s = this->GetHTTPFormStr(name, nameLen);
	if (s == 0)
	{
		return false;
	}
	return s->ToInt16(valOut);
}

Bool Net::WebServer::IWebRequest::GetHTTPFormUInt16(const UTF8Char *name, UOSInt nameLen, UInt16 *valOut)
{
	Text::String *s = this->GetHTTPFormStr(name, nameLen);
	if (s == 0)
	{
		return false;
	}
	return s->ToUInt16(valOut);
}

Bool Net::WebServer::IWebRequest::GetHTTPFormInt32(const UTF8Char *name, UOSInt nameLen, Int32 *valOut)
{
	Text::String *s = this->GetHTTPFormStr(name, nameLen);
	if (s == 0)
	{
		return false;
	}
	return s->ToInt32(valOut);
}

Bool Net::WebServer::IWebRequest::GetHTTPFormUInt32(const UTF8Char *name, UOSInt nameLen, UInt32 *valOut)
{
	Text::String *s = this->GetHTTPFormStr(name, nameLen);
	if (s == 0)
	{
		return false;
	}
	return s->ToUInt32(valOut);
}

Bool Net::WebServer::IWebRequest::GetHTTPFormInt64(const UTF8Char *name, UOSInt nameLen, Int64 *valOut)
{
	Text::String *s = this->GetHTTPFormStr(name, nameLen);
	if (s == 0)
	{
		return false;
	}
	return s->ToInt64(valOut);
}

Bool Net::WebServer::IWebRequest::GetHTTPFormUInt64(const UTF8Char *name, UOSInt nameLen, UInt64 *valOut)
{
	Text::String *s = this->GetHTTPFormStr(name, nameLen);
	if (s == 0)
	{
		return false;
	}
	return s->ToUInt64(valOut);
}

Text::CString Net::WebServer::IWebRequest::GetReqMethodStr()
{
	return Net::WebUtil::RequestMethodGetName(GetReqMethod());
}

Net::BrowserInfo::BrowserType Net::WebServer::IWebRequest::GetBrowser()
{
	if (!this->uaParsed)
		this->ParseUserAgent();
	return this->reqBrowser;
}

const UTF8Char *Net::WebServer::IWebRequest::GetBrowserVer()
{
	if (!this->uaParsed)
		this->ParseUserAgent();
	return this->reqBrowserVer;
}

/*const UTF8Char *Net::WebServer::IWebRequest::GetBrowserMod()
{
	if (!this->uaParsed)
		this->ParseUserAgent();
	return this->reqBrowserMod;
}*/

Manage::OSInfo::OSType Net::WebServer::IWebRequest::GetOS()
{
	if (!this->uaParsed)
		this->ParseUserAgent();
	return this->reqOS;
}

const UTF8Char *Net::WebServer::IWebRequest::GetOSVer()
{
	if (!this->uaParsed)
		this->ParseUserAgent();
	return this->reqOSVer;
}

const Char *Net::WebServer::IWebRequest::RequestProtocolGetName(RequestProtocol reqProto)
{
	switch (reqProto)
	{
	case RequestProtocol::HTTP1_0:
		return "HTTP/1.0";
	case RequestProtocol::HTTP1_1:
		return "HTTP/1.1";
	case RequestProtocol::RTSP1_0:
		return "RTSP/1.0";
	case RequestProtocol::HTTP2_0:
		return "HTTP/2";
	default:
		return "Unknown";
	}
}
