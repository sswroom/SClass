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

	Text::String *uaHdr = this->GetSHeader(CSTR("User-Agent"));
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
			this->reqBrowserVer.v = Text::StrCopyNewC(ent->browserVer, ent->browserVerLen);
			this->reqBrowserVer.leng = ent->browserVerLen;
		}
		else
		{
			this->reqBrowserVer = CSTR_NULL;
		}
		this->reqOS = ent->os;
		if (ent->osVer)
		{
			this->reqOSVer.v = Text::StrCopyNewC(ent->osVer, ent->osVerLen);
			this->reqOSVer.leng = ent->osVerLen;
		}
		else
		{
			this->reqOSVer = CSTR_NULL;
		}
		return;
	}
	Net::UserAgentDB::UAEntry ua;
	Net::UserAgentDB::ParseUserAgent(&ua, uaHdr->ToCString());
	this->reqBrowser = ua.browser;
	this->reqBrowserVer = {ua.browserVer, ua.browserVerLen};
	this->reqOS = ua.os;
	this->reqOSVer = {ua.osVer, ua.osVerLen};
/*	if (ua.osVerLen > 100)
	{
		printf("UA parse error: %s\r\n", uaHdr->v);
	}*/
	SDEL_TEXT(ua.devName);
}

Net::WebServer::IWebRequest::IWebRequest()
{
	this->uaParsed = false;
	this->reqBrowser = Net::BrowserInfo::BT_UNKNOWN;
	this->reqBrowserVer = CSTR_NULL;
	this->reqOS = Manage::OSInfo::OT_UNKNOWN;
	this->reqOSVer = CSTR_NULL;
}

Net::WebServer::IWebRequest::~IWebRequest()
{
	SDEL_TEXT(this->reqBrowserVer.v);
	SDEL_TEXT(this->reqOSVer.v);
}

Bool Net::WebServer::IWebRequest::GetRefererDomain(Text::StringBuilderUTF8 *sb)
{
	Text::String *hdr;
	UTF8Char domain[256];
	UTF8Char *sptr;
	hdr = this->GetSHeader(CSTR("Referer"));
	if (hdr == 0)
	{
		return false;
	}
	sptr = Text::URLString::GetURLDomain(domain, hdr->ToCString(), 0);
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
	Text::String *s = this->GetSHeader(CSTR("If-Modified-Since"));
	if (s)
	{
		if (dt->SetValue(s->ToCString()))
		{
			return true;
		}
	}
	return false;
}

Bool Net::WebServer::IWebRequest::GetCookie(Text::CString name, Text::StringBuilderUTF8 *sb)
{
	Text::String *cookie = this->GetSHeader(CSTR("Cookie"));
	if (cookie == 0)
	{
		return 0;
	}

	UTF8Char *sbuff;
	Text::PString strs[2];
	UOSInt strCnt = 2;
	Bool found = false;

	sbuff = MemAlloc(UTF8Char, cookie->leng + 1);
	cookie->ConcatTo(sbuff);

	strs[1].v = sbuff;
	strs[1].leng = cookie->leng;
	while (strCnt >= 2)
	{
		strCnt = Text::StrSplitTrimP(strs, 2, strs[1], ';');
		if (strs[0].StartsWith(name.v, name.leng) && strs[0].v[name.leng] == '=')
		{
			found = true;
			sb->AppendC(&strs[0].v[name.leng + 1], strs[0].leng - name.leng - 1);
			break;
		}
	}
	MemFree(sbuff);
	return found;
}

Text::String *Net::WebServer::IWebRequest::GetCookieAsNew(Text::CString name)
{
	Text::String *cookie = this->GetSHeader(CSTR("Cookie"));
	if (cookie == 0)
	{
		return 0;
	}

	UTF8Char *sbuff;
	Text::PString strs[2];
	UOSInt strCnt = 2;
	Text::String *ret = 0;

	sbuff = MemAlloc(UTF8Char, cookie->leng + 1);
	cookie->ConcatTo(sbuff);

	strs[1].v = sbuff;
	strs[1].leng = cookie->leng;
	while (strCnt >= 2)
	{
		strCnt = Text::StrSplitTrimP(strs, 2, strs[1], ';');
		if (strs[0].StartsWith(name.v, name.leng) && strs[0].v[name.leng] == '=')
		{
			ret = Text::String::New(&strs[0].v[name.leng + 1], strs[0].leng - name.leng - 1);
			break;
		}
	}
	MemFree(sbuff);
	return ret;
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

UTF8Char *Net::WebServer::IWebRequest::GetQueryValueStr(Text::CString name, UTF8Char *buff, UOSInt buffSize)
{
	Text::String *s = this->GetQueryValue(name);
	if (s == 0)
		return 0;
	return Text::StrConcatCS(buff, s->v, s->leng, buffSize);
}

Bool Net::WebServer::IWebRequest::GetQueryValueI16(Text::CString name, Int16 *val)
{
	Text::String *s = this->GetQueryValue(name);
	if (s == 0)
		return false;
	return s->ToInt16(val);
}

Bool Net::WebServer::IWebRequest::GetQueryValueU16(Text::CString name, UInt16 *val)
{
	Text::String *s = this->GetQueryValue(name);
	if (s == 0)
		return false;
	return s->ToUInt16(val);
}

Bool Net::WebServer::IWebRequest::GetQueryValueI32(Text::CString name, Int32 *val)
{
	Text::String *s = this->GetQueryValue(name);
	if (s == 0)
		return false;
	return s->ToInt32(val);
}

Bool Net::WebServer::IWebRequest::GetQueryValueU32(Text::CString name, UInt32 *val)
{
	Text::String *s = this->GetQueryValue(name);
	if (s == 0)
		return false;
	return s->ToUInt32(val);
}

Bool Net::WebServer::IWebRequest::GetQueryValueI64(Text::CString name, Int64 *val)
{
	Text::String *s = this->GetQueryValue(name);
	if (s == 0)
		return false;
	return s->ToInt64(val);
}

Bool Net::WebServer::IWebRequest::GetQueryValueF64(Text::CString name, Double *val)
{
	Text::String *s = this->GetQueryValue(name);
	if (s == 0)
		return false;
	return s->ToDouble(val);
}

Bool Net::WebServer::IWebRequest::GetHTTPFormInt16(Text::CString name, Int16 *valOut)
{
	Text::String *s = this->GetHTTPFormStr(name);
	if (s == 0)
	{
		return false;
	}
	return s->ToInt16(valOut);
}

Bool Net::WebServer::IWebRequest::GetHTTPFormUInt16(Text::CString name, UInt16 *valOut)
{
	Text::String *s = this->GetHTTPFormStr(name);
	if (s == 0)
	{
		return false;
	}
	return s->ToUInt16(valOut);
}

Bool Net::WebServer::IWebRequest::GetHTTPFormInt32(Text::CString name, Int32 *valOut)
{
	Text::String *s = this->GetHTTPFormStr(name);
	if (s == 0)
	{
		return false;
	}
	return s->ToInt32(valOut);
}

Bool Net::WebServer::IWebRequest::GetHTTPFormUInt32(Text::CString name, UInt32 *valOut)
{
	Text::String *s = this->GetHTTPFormStr(name);
	if (s == 0)
	{
		return false;
	}
	return s->ToUInt32(valOut);
}

Bool Net::WebServer::IWebRequest::GetHTTPFormInt64(Text::CString name, Int64 *valOut)
{
	Text::String *s = this->GetHTTPFormStr(name);
	if (s == 0)
	{
		return false;
	}
	return s->ToInt64(valOut);
}

Bool Net::WebServer::IWebRequest::GetHTTPFormUInt64(Text::CString name, UInt64 *valOut)
{
	Text::String *s = this->GetHTTPFormStr(name);
	if (s == 0)
	{
		return false;
	}
	return s->ToUInt64(valOut);
}

Text::CString Net::WebServer::IWebRequest::RequestProtocolGetName(RequestProtocol reqProto)
{
	switch (reqProto)
	{
	case RequestProtocol::HTTP1_0:
		return CSTR("HTTP/1.0");
	case RequestProtocol::HTTP1_1:
		return CSTR("HTTP/1.1");
	case RequestProtocol::RTSP1_0:
		return CSTR("RTSP/1.0");
	case RequestProtocol::HTTP2_0:
		return CSTR("HTTP/2");
	default:
		return CSTR("Unknown");
	}
}
