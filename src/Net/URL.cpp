#include "Stdafx.h"
#include "IO/FileStream.h"
#include "Net/FTPClient.h"
#include "Net/HTTPClient.h"
#include "Net/RTSPClient.h"
#include "Net/URL.h"
#include "Text/MyString.h"
#include "Text/URLString.h"

IO::ParsedObject *Net::URL::OpenObject(Text::CStringNN url, Text::CString userAgent, NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Data::Duration timeout, NN<IO::LogTool> log)
{
	IO::ParsedObject *pobj;
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	if (url.StartsWithICase(UTF8STRC("http://")))
	{
		NN<Net::HTTPClient> cli = Net::HTTPClient::CreateClient(sockf, ssl, userAgent, true, false);
		cli->SetTimeout(timeout);
		cli->Connect(url, Net::WebUtil::RequestMethod::HTTP_GET, 0, 0, true);
		if (cli->GetRespStatus() == Net::WebStatus::SC_MOVED_TEMPORARILY || cli->GetRespStatus() == Net::WebStatus::SC_MOVED_PERMANENTLY)
		{
			Text::CStringNN newUrl = cli->GetRespHeader(CSTR("Location")).OrEmpty();
			if (newUrl.leng > 0 && !newUrl.Equals(url.v, url.leng) && (newUrl.StartsWith(UTF8STRC("http://")) || newUrl.StartsWith(UTF8STRC("https://"))))
			{
				pobj = OpenObject(newUrl, userAgent, sockf, ssl, timeout, log);
				cli.Delete();
				return pobj;
			}
		}
		return cli.Ptr();
	}
	else if (url.StartsWithICase(UTF8STRC("https://")))
	{
		NN<Net::HTTPClient> cli = Net::HTTPClient::CreateClient(sockf, ssl, userAgent, true, true);
		cli->SetTimeout(timeout);
		cli->Connect(url, Net::WebUtil::RequestMethod::HTTP_GET, 0, 0, true);
		if (cli->GetRespStatus() == Net::WebStatus::SC_MOVED_TEMPORARILY || cli->GetRespStatus() == Net::WebStatus::SC_MOVED_PERMANENTLY)
		{
			Text::CStringNN newUrl = cli->GetRespHeader(CSTR("Location")).OrEmpty();
			if (newUrl.leng > 0 && !newUrl.Equals(url.v, url.leng) && (newUrl.StartsWith(UTF8STRC("http://")) || newUrl.StartsWith(UTF8STRC("https://"))))
			{
				pobj = OpenObject(newUrl, userAgent, sockf, ssl, timeout, log);
				cli.Delete();
				return pobj;
			}
		}
		return cli.Ptr();
	}
	else if (url.StartsWithICase(UTF8STRC("file:///")))
	{
		sbuff[0] = 0;
		sptr = Text::URLString::GetURLFilePath(sbuff, url.v, url.leng).Or(sbuff);
		NEW_CLASS(pobj, IO::FileStream(CSTRP(sbuff, sptr), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		return pobj;
	}
	else if (url.StartsWithICase(UTF8STRC("ftp://")))
	{
		NEW_CLASS(pobj, Net::FTPClient(url, sockf, true, 0, timeout));
		return pobj;
	}
	else if (url.StartsWithICase(UTF8STRC("rtsp://")))
	{
		pobj = Net::RTSPClient::ParseURL(sockf, url, timeout, log);
		return pobj;
	}
	return 0;
}
