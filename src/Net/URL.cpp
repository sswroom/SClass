#include "Stdafx.h"
#include "IO/FileStream.h"
#include "Net/FTPClient.h"
#include "Net/HTTPClient.h"
#include "Net/RTSPClient.h"
#include "Net/URL.h"
#include "Text/MyString.h"
#include "Text/URLString.h"

IO::ParsedObject *Net::URL::OpenObject(Text::CString url, Text::CString userAgent, Net::SocketFactory *sockf, Net::SSLEngine *ssl)
{
	IO::ParsedObject *pobj;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	if (url.StartsWithICase(UTF8STRC("http://")))
	{
		Net::HTTPClient *cli = Net::HTTPClient::CreateClient(sockf, ssl, userAgent, true, false);
		cli->Connect(url, Net::WebUtil::RequestMethod::HTTP_GET, 0, 0, true);
		return cli;
	}
	else if (url.StartsWithICase(UTF8STRC("https://")))
	{
		Net::HTTPClient *cli = Net::HTTPClient::CreateClient(sockf, ssl, userAgent, true, true);
		cli->Connect(url, Net::WebUtil::RequestMethod::HTTP_GET, 0, 0, true);
		return cli;
	}
	else if (url.StartsWithICase(UTF8STRC("file:///")))
	{
		sptr = Text::URLString::GetURLFilePath(sbuff, url.v, url.leng);
		NEW_CLASS(pobj, IO::FileStream(CSTRP(sbuff, sptr), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		return pobj;
	}
	else if (url.StartsWithICase(UTF8STRC("ftp://")))
	{
		NEW_CLASS(pobj, Net::FTPClient(url, sockf, true, 0));
		return pobj;
	}
	else if (url.StartsWithICase(UTF8STRC("rtsp://")))
	{
		pobj = Net::RTSPClient::ParseURL(sockf, url);
		return pobj;
	}
	return 0;
}
