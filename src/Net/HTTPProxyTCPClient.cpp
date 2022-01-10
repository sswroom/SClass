#include "Stdafx.h"

#include "MyMemory.h"
#include "Crypto/Encrypt/Base64.h"
#include "Net/HTTPProxyTCPClient.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

Net::HTTPProxyTCPClient::HTTPProxyTCPClient(Net::SocketFactory *sockf, const UTF8Char *proxyHost, UInt16 proxyPort, PasswordType pt, const UTF8Char *userName, const UTF8Char *pwd, const UTF8Char *destHost, UInt16 destPort) : Net::TCPClient(sockf, (Socket*)0)
{
	this->SetSourceName(destHost, Text::StrCharCnt(destHost));

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
	if (!sockf->Connect(s, &addr, proxyPort))
	{
		sockf->DestroySocket(s);
		this->s = 0;
		this->flags |= 12;
		return;
	}
	
	Char reqBuff[512];
	Char userPwd[256];
	Char *sptr = Text::StrConcat(reqBuff, "CONNECT ");
	Char *sptr2;
	while ((*sptr++ = (Char)*destHost++) != 0);
	sptr[-1] = ':';
	sptr = Text::StrInt32(sptr, destPort);
	sptr = Text::StrConcat(sptr, " HTTP/1.1\r\n");
	if (pt == PWDT_BASIC)
	{
		sptr2 = (Char*)Text::StrConcat((UTF8Char*)userPwd, userName);
		*sptr2++ = ':';
		sptr2 = (Char*)Text::StrConcat((UTF8Char*)sptr2, pwd);

		sptr = Text::StrConcat(sptr, "Proxy-Authorization: Basic ");
		Crypto::Encrypt::Base64 b64;
		sptr = sptr + b64.Encrypt((UInt8*)userPwd, (UOSInt)(sptr2 - userPwd), (UInt8*)sptr, 0);
		sptr = Text::StrConcat(sptr, "\r\n");
	}
	sptr = Text::StrConcat(sptr, "\r\n");
	this->Write((UInt8*)reqBuff, (UOSInt)(sptr - reqBuff));
	this->SetTimeout(4000);
	this->Read((UInt8*)reqBuff, 512);
	this->SetTimeout(-1);

	if (Text::StrStartsWith(reqBuff, "HTTP/1.1 200"))
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
