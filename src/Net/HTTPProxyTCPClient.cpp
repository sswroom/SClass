#include "Stdafx.h"

#include "MyMemory.h"
#include "Crypto/Encrypt/Base64.h"
#include "Net/HTTPProxyTCPClient.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

//#define VERBOSE

Net::HTTPProxyTCPClient::HTTPProxyTCPClient(NN<Net::SocketFactory> sockf, Text::CStringNN proxyHost, UInt16 proxyPort, PasswordType pt, UnsafeArrayOpt<const UTF8Char> userName, UnsafeArrayOpt<const UTF8Char> pwd, Text::CStringNN destHost, UInt16 destPort) : Net::TCPClient(sockf, nullptr)
{
	this->SetSourceName(destHost);

	Net::SocketUtil::AddressInfo addr;
	if (!sockf->DNSResolveIP(proxyHost, addr))
	{
#if defined(VERBOSE)
		printf("HTTPProxyTCP: Error in resolving host: %s\r\n", proxyHost.v.Ptr());
#endif
		this->flags |= 12;
		return;
	}
	NN<Socket> s;
	if (addr.addrType == Net::AddrType::IPv4)
	{
		this->s = sockf->CreateTCPSocketv4();
		if (!this->s.SetTo(s))
		{
#if defined(VERBOSE)
			printf("HTTPProxyTCP: Error in creating v4 socket\r\n");
#endif
			this->flags |= 12;
			return;
		}
	}
	else if (addr.addrType == Net::AddrType::IPv6)
	{
		this->s = sockf->CreateTCPSocketv6();
		if (!this->s.SetTo(s))
		{
#if defined(VERBOSE)
			printf("HTTPProxyTCP: Error in creating v6 socket\r\n");
#endif
			this->flags |= 12;
			return;
		}
	}
	else
	{
#if defined(VERBOSE)
		printf("HTTPProxyTCP: Unknown address type\r\n");
#endif
		this->flags |= 12;
		return;
	}
	if (!sockf->Connect(s, addr, proxyPort, 15000))
	{
#if defined(VERBOSE)
		printf("HTTPProxyTCP: Error in connecting to proxy server\r\n");
#endif
		sockf->DestroySocket(s);
		this->s = nullptr;
		this->flags |= 12;
		return;
	}
	this->flags = 0;
	this->cliId = sockf->GenSocketId(s);
	
	UTF8Char reqBuff[512];
	UTF8Char userPwd[256];
	UnsafeArray<UTF8Char> sptr = Text::StrConcatC(reqBuff, UTF8STRC("CONNECT "));
	UnsafeArray<UTF8Char> sptr2;
	UOSInt respSize;
	sptr = destHost.ConcatTo(sptr);
	*sptr++ = ':';
	sptr = Text::StrUInt16(sptr, destPort);
	sptr = Text::StrConcatC(sptr, UTF8STRC(" HTTP/1.1\r\n"));
	sptr = Text::StrConcatC(sptr, UTF8STRC("Host: "));
	sptr = destHost.ConcatTo(sptr);
	*sptr++ = ':';
	sptr = Text::StrUInt16(sptr, destPort);
	sptr = Text::StrConcatC(sptr, UTF8STRC("\r\n"));
	UnsafeArray<const UTF8Char> nnuserName;
	UnsafeArray<const UTF8Char> nnpwd;
	if (pt == PWDT_BASIC && userName.SetTo(nnuserName) && pwd.SetTo(nnpwd))
	{
		sptr2 = Text::StrConcat(userPwd, nnuserName);
		*sptr2++ = ':';
		sptr2 = Text::StrConcat(sptr2, nnpwd);

		sptr = Text::StrConcatC(sptr, UTF8STRC("Proxy-Authorization: Basic "));
		Crypto::Encrypt::Base64 b64;
		sptr = sptr + b64.Encrypt((UInt8*)userPwd, (UOSInt)(sptr2 - userPwd), sptr);
		sptr = Text::StrConcatC(sptr, UTF8STRC("\r\n"));
	}
	sptr = Text::StrConcatC(sptr, UTF8STRC("\r\n"));
#if defined(VERBOSE)
	printf("HTTPProxyTCP: Sending:\r\n%s", reqBuff);
#endif
	this->Write(Data::ByteArrayR((UInt8*)reqBuff, (UOSInt)(sptr - reqBuff)));
	this->SetTimeout(4000);
	respSize = this->Read(BYTEARR(reqBuff));
	this->SetTimeout(60000);

#if defined(VERBOSE)
	reqBuff[respSize] = 0;
	printf("HTTPProxyTCP: Recv:\r\n%s", reqBuff);
#endif

	if (Text::StrStartsWithC(reqBuff, respSize, UTF8STRC("HTTP/1.1 200")))
	{
	}
	else if (Text::StrStartsWithC(reqBuff, respSize, UTF8STRC("HTTP/1.0 200")))
	{
	}
	else
	{
#if defined(VERBOSE)
		printf("HTTPProxyTCP: Unknown response\r\n");
#endif
		sockf->DestroySocket(s);
		this->s = nullptr;
		this->flags |= 12;
		return;
	}
	this->cliId = sockf->GenSocketId(s);
}

Net::HTTPProxyTCPClient::~HTTPProxyTCPClient()
{
}
