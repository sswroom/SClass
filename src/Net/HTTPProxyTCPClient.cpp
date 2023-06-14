#include "Stdafx.h"

#include "MyMemory.h"
#include "Crypto/Encrypt/Base64.h"
#include "Net/HTTPProxyTCPClient.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

Net::HTTPProxyTCPClient::HTTPProxyTCPClient(Net::SocketFactory *sockf, Text::CString proxyHost, UInt16 proxyPort, PasswordType pt, const UTF8Char *userName, const UTF8Char *pwd, Text::CString destHost, UInt16 destPort) : Net::TCPClient(sockf, (Socket*)0)
{
	this->SetSourceName(destHost);

	Net::SocketUtil::AddressInfo addr;
	if (!sockf->DNSResolveIP(proxyHost, &addr))
	{
		this->flags |= 12;
		return;
	}
	if (addr.addrType == Net::AddrType::IPv4)
	{
		this->s = sockf->CreateTCPSocketv4();
		if (this->s == 0)
		{
			this->flags |= 12;
			return;
		}
	}
	else if (addr.addrType == Net::AddrType::IPv4)
	{
		this->s = sockf->CreateTCPSocketv6();
		if (this->s == 0)
		{
			this->flags |= 12;
			return;
		}
	}
	else
	{
		this->flags |= 12;
		return;
	}
	if (!sockf->Connect(s, &addr, proxyPort, 15000))
	{
		sockf->DestroySocket(s);
		this->s = 0;
		this->flags |= 12;
		return;
	}
	
	UTF8Char reqBuff[512];
	UTF8Char userPwd[256];
	UTF8Char *sptr = Text::StrConcatC(reqBuff, UTF8STRC("CONNECT "));
	UTF8Char *sptr2;
	UOSInt respSize;
	sptr = destHost.ConcatTo(sptr);
	*sptr++ = ':';
	sptr = Text::StrUInt16(sptr, destPort);
	sptr = Text::StrConcatC(sptr, UTF8STRC(" HTTP/1.1\r\n"));
	if (pt == PWDT_BASIC)
	{
		sptr2 = Text::StrConcat(userPwd, userName);
		*sptr2++ = ':';
		sptr2 = Text::StrConcat(sptr2, pwd);

		sptr = Text::StrConcatC(sptr, UTF8STRC("Proxy-Authorization: Basic "));
		Crypto::Encrypt::Base64 b64;
		sptr = sptr + b64.Encrypt((UInt8*)userPwd, (UOSInt)(sptr2 - userPwd), (UInt8*)sptr, 0);
		sptr = Text::StrConcatC(sptr, UTF8STRC("\r\n"));
	}
	sptr = Text::StrConcatC(sptr, UTF8STRC("\r\n"));
	this->Write((UInt8*)reqBuff, (UOSInt)(sptr - reqBuff));
	this->SetTimeout(4000);
	respSize = this->Read((UInt8*)reqBuff, 512);
	this->SetTimeout(-1);

	if (Text::StrStartsWithC(reqBuff, respSize, UTF8STRC("HTTP/1.1 200")))
	{

	}
	else
	{
		sockf->DestroySocket(s);
		this->s = 0;
		this->flags |= 12;
		return;
	}
	this->cliId = sockf->GenSocketId(s);
}
