#include "Stdafx.h"
#include "IO/FileStream.h"
#include "Net/FTPClient.h"
#include "Net/HTTPClient.h"
#include "Net/RTSPClient.h"
#include "Net/URL.h"
#include "Text/MyString.h"
#include "Text/URLString.h"

IO::ParsedObject *Net::URL::OpenObject(const UTF8Char *url, const UTF8Char *userAgent, Net::SocketFactory *sockf, Net::SSLEngine *ssl)
{
	IO::ParsedObject *pobj;
	UTF8Char sbuff[512];
	if (Text::StrStartsWithICase(url, (const UTF8Char*)"HTTP://"))
	{
		Net::HTTPClient *cli = Net::HTTPClient::CreateClient(sockf, ssl, userAgent, true, false);
		cli->Connect(url, "GET", 0, 0, true);
		return cli;
	}
	else if (Text::StrStartsWithICase(url, (const UTF8Char*)"HTTPS://"))
	{
		Net::HTTPClient *cli = Net::HTTPClient::CreateClient(sockf, ssl, userAgent, true, true);
		cli->Connect(url, "GET", 0, 0, true);
		return cli;
	}
	else if (Text::StrStartsWithICase(url, (const UTF8Char*)"FILE:///"))
	{
		Text::URLString::GetURLFilePath(sbuff, url);
		NEW_CLASS(pobj, IO::FileStream(sbuff, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		return pobj;
	}
	else if (Text::StrStartsWithICase(url, (const UTF8Char*)"FTP://"))
	{
		NEW_CLASS(pobj, Net::FTPClient(url, sockf, true, 0));
		return pobj;
	}
	else if (Text::StrStartsWithICase(url, (const UTF8Char*)"RTSP://"))
	{
		pobj = Net::RTSPClient::ParseURL(sockf, url);
		return pobj;
	}
	return 0;
}
