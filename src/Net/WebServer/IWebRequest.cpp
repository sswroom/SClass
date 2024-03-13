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

	NotNullPtr<Text::String> uaHdr;
	if (!this->GetSHeader(CSTR("User-Agent")).SetTo(uaHdr))
	{
		return;
	}
	const Net::UserAgentDB::UAEntry *ent = Net::UserAgentDB::GetUserAgentInfo(uaHdr->v);
	if (ent)
	{
		this->reqBrowser = ent->browser;
		if (ent->browserVer)
		{
			this->reqBrowserVer.v = Text::StrCopyNewC(ent->browserVer, ent->browserVerLen).Ptr();
			this->reqBrowserVer.leng = ent->browserVerLen;
		}
		else
		{
			this->reqBrowserVer = CSTR_NULL;
		}
		this->reqOS = ent->os;
		if (ent->osVer)
		{
			this->reqOSVer.v = Text::StrCopyNewC(ent->osVer, ent->osVerLen).Ptr();
			this->reqOSVer.leng = ent->osVerLen;
		}
		else
		{
			this->reqOSVer = CSTR_NULL;
		}
		if (ent->devName)
		{
			this->reqDevModel.v = Text::StrCopyNewC(ent->devName, ent->devNameLen).Ptr();
			this->reqDevModel.leng = ent->devNameLen;
		}
		return;
	}
	Net::UserAgentDB::UAEntry ua;
	Net::UserAgentDB::ParseUserAgent(&ua, uaHdr->ToCString());
	this->reqBrowser = ua.browser;
	this->reqBrowserVer = {ua.browserVer, ua.browserVerLen};
	this->reqOS = ua.os;
	this->reqOSVer = {ua.osVer, ua.osVerLen};
	this->reqDevModel = {ua.devName, ua.devNameLen};
}

Net::WebServer::IWebRequest::IWebRequest()
{
	this->uaParsed = false;
	this->reqBrowser = Net::BrowserInfo::BT_UNKNOWN;
	this->reqBrowserVer = CSTR_NULL;
	this->reqOS = Manage::OSInfo::OT_UNKNOWN;
	this->reqOSVer = CSTR_NULL;
	this->reqDevModel = CSTR_NULL;
}

Net::WebServer::IWebRequest::~IWebRequest()
{
	SDEL_TEXT(this->reqBrowserVer.v);
	SDEL_TEXT(this->reqOSVer.v);
	SDEL_TEXT(this->reqDevModel.v);
}

Bool Net::WebServer::IWebRequest::GetRefererDomain(NotNullPtr<Text::StringBuilderUTF8> sb) const
{
	NotNullPtr<Text::String> hdr;
	UTF8Char domain[256];
	UTF8Char *sptr;
	if (!this->GetSHeader(CSTR("Referer")).SetTo(hdr))
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

Bool Net::WebServer::IWebRequest::GetIfModifiedSince(NotNullPtr<Data::DateTime> dt) const
{
	NotNullPtr<Text::String> s;
	if (this->GetSHeader(CSTR("If-Modified-Since")).SetTo(s))
	{
		if (dt->SetValue(s->ToCString()))
		{
			return true;
		}
	}
	return false;
}

Bool Net::WebServer::IWebRequest::GetCookie(Text::CStringNN name, NotNullPtr<Text::StringBuilderUTF8> sb) const
{
	NotNullPtr<Text::String> cookie;
	if (!this->GetSHeader(CSTR("Cookie")).SetTo(cookie))
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

Optional<Text::String> Net::WebServer::IWebRequest::GetCookieAsNew(Text::CStringNN name) const
{
	NotNullPtr<Text::String> cookie;
	if (!this->GetSHeader(CSTR("Cookie")).SetTo(cookie))
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
			ret = Text::String::New(&strs[0].v[name.leng + 1], strs[0].leng - name.leng - 1).Ptr();
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
	NotNullPtr<Text::String> s = this->GetRequestURI();
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

UTF8Char *Net::WebServer::IWebRequest::GetQueryValueStr(Text::CStringNN name, UTF8Char *buff, UOSInt buffSize)
{
	NotNullPtr<Text::String> s;
	if (!this->GetQueryValue(name).SetTo(s))
		return 0;
	return Text::StrConcatCS(buff, s->v, s->leng, buffSize);
}

Bool Net::WebServer::IWebRequest::GetQueryValueI16(Text::CStringNN name, OutParam<Int16> val)
{
	NotNullPtr<Text::String> s;
	if (!this->GetQueryValue(name).SetTo(s))
		return false;
	return s->ToInt16(val);
}

Bool Net::WebServer::IWebRequest::GetQueryValueU16(Text::CStringNN name, OutParam<UInt16> val)
{
	NotNullPtr<Text::String> s;
	if (!this->GetQueryValue(name).SetTo(s))
		return false;
	return s->ToUInt16(val);
}

Bool Net::WebServer::IWebRequest::GetQueryValueI32(Text::CStringNN name, OutParam<Int32> val)
{
	NotNullPtr<Text::String> s;
	if (!this->GetQueryValue(name).SetTo(s))
		return false;
	return s->ToInt32(val);
}

Bool Net::WebServer::IWebRequest::GetQueryValueU32(Text::CStringNN name, OutParam<UInt32> val)
{
	NotNullPtr<Text::String> s;
	if (!this->GetQueryValue(name).SetTo(s))
		return false;
	return s->ToUInt32(val);
}

Bool Net::WebServer::IWebRequest::GetQueryValueI64(Text::CStringNN name, OutParam<Int64> val)
{
	NotNullPtr<Text::String> s;
	if (!this->GetQueryValue(name).SetTo(s))
		return false;
	return s->ToInt64(val);
}

Bool Net::WebServer::IWebRequest::GetQueryValueF64(Text::CStringNN name, OutParam<Double> val)
{
	NotNullPtr<Text::String> s;
	if (!this->GetQueryValue(name).SetTo(s))
		return false;
	return s->ToDouble(val);
}

Bool Net::WebServer::IWebRequest::GetHTTPFormInt16(Text::CStringNN name, OutParam<Int16> valOut)
{
	NotNullPtr<Text::String> s;
	if (!this->GetHTTPFormStr(name).SetTo(s))
	{
		return false;
	}
	return s->ToInt16(valOut);
}

Bool Net::WebServer::IWebRequest::GetHTTPFormUInt16(Text::CStringNN name, OutParam<UInt16> valOut)
{
	NotNullPtr<Text::String> s;
	if (!this->GetHTTPFormStr(name).SetTo(s))
	{
		return false;
	}
	return s->ToUInt16(valOut);
}

Bool Net::WebServer::IWebRequest::GetHTTPFormInt32(Text::CStringNN name, OutParam<Int32> valOut)
{
	NotNullPtr<Text::String> s;
	if (!this->GetHTTPFormStr(name).SetTo(s))
	{
		return false;
	}
	return s->ToInt32(valOut);
}

Bool Net::WebServer::IWebRequest::GetHTTPFormUInt32(Text::CStringNN name, OutParam<UInt32> valOut)
{
	NotNullPtr<Text::String> s;
	if (!this->GetHTTPFormStr(name).SetTo(s))
	{
		return false;
	}
	return s->ToUInt32(valOut);
}

Bool Net::WebServer::IWebRequest::GetHTTPFormInt64(Text::CStringNN name, OutParam<Int64> valOut)
{
	NotNullPtr<Text::String> s;
	if (!this->GetHTTPFormStr(name).SetTo(s))
	{
		return false;
	}
	return s->ToInt64(valOut);
}

Bool Net::WebServer::IWebRequest::GetHTTPFormUInt64(Text::CStringNN name, OutParam<UInt64> valOut)
{
	NotNullPtr<Text::String> s;
	if (!this->GetHTTPFormStr(name).SetTo(s))
	{
		return false;
	}
	return s->ToUInt64(valOut);
}

Bool Net::WebServer::IWebRequest::GetHTTPFormDouble(Text::CStringNN name, OutParam<Double> valOut)
{
	NotNullPtr<Text::String> s;
	if (!this->GetHTTPFormStr(name).SetTo(s))
	{
		return false;
	}
	return s->ToDouble(valOut);
}

UTF8Char *Net::WebServer::IWebRequest::BuildURLHost(UTF8Char *sbuff)
{
	NotNullPtr<Text::String> s;
	if (this->GetSHeader(CSTR("X-Forwarded-Proto")).SetTo(s))
	{
		sbuff = s->ConcatTo(sbuff);
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("://"));
	}
	else if (this->GetSHeader(CSTR("X-Forwarded-Protocol")).SetTo(s))
	{
		sbuff = s->ConcatTo(sbuff);
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("://"));
	}
	else if (this->GetSHeader(CSTR("X-Url-Scheme")).SetTo(s))
	{
		sbuff = s->ConcatTo(sbuff);
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("://"));
	}
	else if (this->GetSHeader(CSTR("X-Forwarded-Ssl")).SetTo(s))
	{
		if (s->Equals(CSTR("on")))
		{
			sbuff = Text::StrConcatC(sbuff, UTF8STRC("https://"));
		}
		else
		{
			sbuff = Text::StrConcatC(sbuff, UTF8STRC("http://"));
		}
	}
	else if (this->IsSecure())
	{
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("https://"));
	}
	else
	{
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("http://"));
	}

	if (this->GetSHeader(CSTR("X-Forwarded-Host")).SetTo(s))
	{
		sbuff = s->ConcatTo(sbuff);
	}
	else if (this->GetSHeader(CSTR("Host")).SetTo(s))
	{
		sbuff = s->ConcatTo(sbuff);
	}
	else
	{
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("localhost"));
	}
	return sbuff;
}

void Net::WebServer::IWebRequest::GetRequestAddr(NotNullPtr<Net::SocketUtil::AddressInfo> addr) const
{
	Text::StringBuilderUTF8 sb;
	Text::PString s;
	Text::PString nextStr;
	UOSInt i;
	if (this->GetHeaderC(sb, CSTR("Forwarded")))
	{
		s = sb;
		while (true)
		{
			i = s.BranketSearch(0, ',');
			if (i == INVALID_INDEX)
				break;
			s = s.SubstrTrim(i + 1);
		}
		while (true)
		{
			i = s.BranketSearch(0, ';');
			if (i == INVALID_INDEX)
				nextStr.v = 0;
			else
			{
				nextStr = s.SubstrTrim(i + 1);
				s.TrimToLength(i);
			}

			if (s.StartsWithICase(UTF8STRC("for=")))
			{
				if (s.v[4] == '"')
				{
					i = s.IndexOf(5, '"');
					if (i != INVALID_INDEX)
					{
						s = s.Substring(5);
						s.TrimToLength(i - 5);
					}
					else
					{
						s = s.Substring(4);
					}
				}
				else
				{
					s = s.Substring(4);
				}
				if (Net::SocketUtil::SetAddrInfo(addr, s.ToCString()))
					return;
			}

			if (nextStr.v == 0)
				break;
			s = nextStr;
		}
	}

	sb.ClearStr();
	if (this->GetHeaderC(sb, CSTR("X-Forwarded-For")))
	{
		if (Net::SocketUtil::SetAddrInfo(addr, sb.ToCString()))
			return;
	}
	addr.SetVal(this->GetClientAddr().Ptr()[0]);
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
