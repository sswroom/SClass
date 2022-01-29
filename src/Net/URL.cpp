#include "Stdafx.h"
#include "IO/FileStream.h"
#include "Net/FTPClient.h"
#include "Net/HTTPClient.h"
#include "Net/RTSPClient.h"
#include "Net/URL.h"
#include "Text/MyString.h"
#include "Text/URLString.h"

IO::ParsedObject *Net::URL::OpenObject(const UTF8Char *url, Text::CString userAgent, Net::SocketFactory *sockf, Net::SSLEngine *ssl)
{
	IO::ParsedObject *pobj;
	UTF8Char sbuff[512];
	UOSInt urlLen = Text::StrCharCnt(url);
	if (Text::StrStartsWithICaseC(url, urlLen, UTF8STRC("http://")))
	{
		Net::HTTPClient *cli = Net::HTTPClient::CreateClient(sockf, ssl, userAgent, true, false);
		cli->Connect({url, urlLen}, Net::WebUtil::RequestMethod::HTTP_GET, 0, 0, true);
		return cli;
	}
	else if (Text::StrStartsWithICaseC(url, urlLen, UTF8STRC("https://")))
	{
		Net::HTTPClient *cli = Net::HTTPClient::CreateClient(sockf, ssl, userAgent, true, true);
		cli->Connect({url, urlLen}, Net::WebUtil::RequestMethod::HTTP_GET, 0, 0, true);
		return cli;
	}
	else if (Text::StrStartsWithICaseC(url, urlLen, UTF8STRC("file:///")))
	{
		Text::URLString::GetURLFilePath(sbuff, url, urlLen);
		NEW_CLASS(pobj, IO::FileStream(sbuff, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		return pobj;
	}
	else if (Text::StrStartsWithICaseC(url, urlLen, UTF8STRC("ftp://")))
	{
		NEW_CLASS(pobj, Net::FTPClient(url, sockf, true, 0));
		return pobj;
	}
	else if (Text::StrStartsWithICaseC(url, urlLen, UTF8STRC("rtsp://")))
	{
		pobj = Net::RTSPClient::ParseURL(sockf, url, urlLen);
		return pobj;
	}
	return 0;
}
