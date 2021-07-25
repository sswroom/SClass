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
	UTF8Char sbuff[512];
	this->uaParsed = true;
	this->reqBrowser = Net::BrowserInfo::BT_UNKNOWN;

	if (this->GetHeader(sbuff, (const UTF8Char*)"User-Agent", 512) == 0)
	{
		return;
	}
	const Net::UserAgentDB::UAEntry *ent = Net::UserAgentDB::GetUserAgentInfo(sbuff);
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
	Net::UserAgentDB::ParseUserAgent(&ua, sbuff);
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

Bool Net::WebServer::IWebRequest::GetRefererDomain(Text::StringBuilderUTF *sb)
{
	Text::StringBuilderUTF8 hdr;
	UTF8Char domain[256];
	UTF8Char *sptr;
	this->GetHeader(&hdr, (const UTF8Char*)"Referer");
	sptr = Text::URLString::GetURLDomain(domain, hdr.ToString(), 0);
	if (sptr == 0)
	{
		return false;
	}
	else
	{
		sb->Append(domain);
		return true;
	}
}

Bool Net::WebServer::IWebRequest::GetIfModifiedSince(Data::DateTime *dt)
{
	Text::StringBuilderUTF8 hdr;
	if (this->GetHeader(&hdr, (const UTF8Char*)"If-Modified-Since"))
	{
		if (dt->SetValue(hdr.ToString()))
		{
			return true;
		}
	}
	return false;
}

UTF8Char *Net::WebServer::IWebRequest::GetRequestPath(UTF8Char *sbuff, UOSInt maxLeng)
{
	const UTF8Char *uri = this->GetRequestURI();
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
	const UTF8Char *uri = this->GetRequestURI();
	UOSInt i = Text::StrIndexOf(uri, '?');
	if (i == INVALID_INDEX)
		return 0;
	uri = &uri[i + 1];

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

UTF8Char *Net::WebServer::IWebRequest::GetQueryValueStr(const UTF8Char *name, UTF8Char *buff, UOSInt buffSize)
{
	const UTF8Char *csptr = this->GetQueryValue(name);
	if (csptr == 0)
		return 0;
	return Text::StrConcatS(buff, csptr, buffSize);
}

Bool Net::WebServer::IWebRequest::GetQueryValueI16(const UTF8Char *name, Int16 *val)
{
	const UTF8Char *csptr = this->GetQueryValue(name);
	if (csptr == 0)
		return false;
	return Text::StrToInt16(csptr, val);
}

Bool Net::WebServer::IWebRequest::GetQueryValueU16(const UTF8Char *name, UInt16 *val)
{
	const UTF8Char *csptr = this->GetQueryValue(name);
	if (csptr == 0)
		return false;
	return Text::StrToUInt16(csptr, val);
}

Bool Net::WebServer::IWebRequest::GetQueryValueI32(const UTF8Char *name, Int32 *val)
{
	const UTF8Char *csptr = this->GetQueryValue(name);
	if (csptr == 0)
		return false;
	return Text::StrToInt32(csptr, val);
}

Bool Net::WebServer::IWebRequest::GetQueryValueU32(const UTF8Char *name, UInt32 *val)
{
	const UTF8Char *csptr = this->GetQueryValue(name);
	if (csptr == 0)
		return false;
	return Text::StrToUInt32(csptr, val);
}

Bool Net::WebServer::IWebRequest::GetQueryValueI64(const UTF8Char *name, Int64 *val)
{
	const UTF8Char *csptr = this->GetQueryValue(name);
	if (csptr == 0)
		return false;
	return Text::StrToInt64(csptr, val);
}

Bool Net::WebServer::IWebRequest::GetQueryValueF64(const UTF8Char *name, Double *val)
{
	const UTF8Char *csptr = this->GetQueryValue(name);
	if (csptr == 0)
		return false;
	return Text::StrToDouble(csptr, val);
}

Bool Net::WebServer::IWebRequest::GetHTTPFormInt16(const UTF8Char *name, Int16 *valOut)
{
	const UTF8Char *csptr = this->GetHTTPFormStr(name);
	if (csptr == 0)
	{
		return false;
	}
	return Text::StrToInt16(csptr, valOut);
}

Bool Net::WebServer::IWebRequest::GetHTTPFormUInt16(const UTF8Char *name, UInt16 *valOut)
{
	const UTF8Char *csptr = this->GetHTTPFormStr(name);
	if (csptr == 0)
	{
		return false;
	}
	return Text::StrToUInt16(csptr, valOut);
}

Bool Net::WebServer::IWebRequest::GetHTTPFormInt32(const UTF8Char *name, Int32 *valOut)
{
	const UTF8Char *csptr = this->GetHTTPFormStr(name);
	if (csptr == 0)
	{
		return false;
	}
	return Text::StrToInt32(csptr, valOut);
}

Bool Net::WebServer::IWebRequest::GetHTTPFormUInt32(const UTF8Char *name, UInt32 *valOut)
{
	const UTF8Char *csptr = this->GetHTTPFormStr(name);
	if (csptr == 0)
	{
		return false;
	}
	return Text::StrToUInt32(csptr, valOut);
}

Bool Net::WebServer::IWebRequest::GetHTTPFormInt64(const UTF8Char *name, Int64 *valOut)
{
	const UTF8Char *csptr = this->GetHTTPFormStr(name);
	if (csptr == 0)
	{
		return false;
	}
	return Text::StrToInt64(csptr, valOut);
}

Bool Net::WebServer::IWebRequest::GetHTTPFormUInt64(const UTF8Char *name, UInt64 *valOut)
{
	const UTF8Char *csptr = this->GetHTTPFormStr(name);
	if (csptr == 0)
	{
		return false;
	}
	return Text::StrToUInt64(csptr, valOut);
}

const UTF8Char *Net::WebServer::IWebRequest::GetReqMethodStr()
{
	return RequestMethodGetName(GetReqMethod());
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

const UTF8Char *Net::WebServer::IWebRequest::RequestMethodGetName(RequestMethod reqMeth)
{
	switch (reqMeth)
	{
	case REQMETH_HTTP_GET:
		return (const UTF8Char*)"GET";
	case REQMETH_HTTP_POST:
		return (const UTF8Char*)"POST";
	case REQMETH_HTTP_PUT:
		return (const UTF8Char*)"PUT";
	case REQMETH_HTTP_PATCH:
		return (const UTF8Char*)"PATCH";
	case REQMETH_HTTP_DELETE:
		return (const UTF8Char*)"DELETE";
	case REQMETH_HTTP_CONNECT:
		return (const UTF8Char*)"CONNECT";
	case REQMETH_RTSP_DESCRIBE:
		return (const UTF8Char*)"DESCRIBE";
	case REQMETH_RTSP_ANNOUNCE:
		return (const UTF8Char*)"ANNOUNCE";
	case REQMETH_RTSP_GET_PARAMETER:
		return (const UTF8Char*)"GET_PARAMETER";
	case REQMETH_RTSP_OPTIONS:
		return (const UTF8Char*)"OPTIONS";
	case REQMETH_RTSP_PAUSE:
		return (const UTF8Char*)"PAUSE";
	case REQMETH_RTSP_PLAY:
		return (const UTF8Char*)"PLAY";
	case REQMETH_RTSP_RECORD:
		return (const UTF8Char*)"RECORD";
	case REQMETH_RTSP_REDIRECT:
		return (const UTF8Char*)"REDIRECT";
	case REQMETH_RTSP_SETUP:
		return (const UTF8Char*)"SETUP";
	case REQMETH_RTSP_SET_PARAMETER:
		return (const UTF8Char*)"SET_PARAMETER";
	case REQMETH_RTSP_TEARDOWN:
		return (const UTF8Char*)"TEARDOWN";
	default:
		return (const UTF8Char*)"?";
	}
}
