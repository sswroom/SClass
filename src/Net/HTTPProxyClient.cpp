#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Encrypt/Base64.h"
#include "Data/ArrayList.h"
#include "Data/ByteTool.h"
#include "Data/DateTime.h"
#include "IO/Stream.h"
#include "Net/SocketFactory.h"
#include "Net/TCPClient.h"
#include "Net/HTTPProxyClient.h"
#include "Sync/Event.h"
#include "Text/Encoding.h"
#include "Text/EncodingFactory.h"
#include "Text/MyString.h"

//#define SHOWDEBUG
//#define DEBUGSPEED
#if defined(SHOWDEBUG) || defined(DEBUGSPEED)
#include <stdio.h>
#endif

#define BUFFSIZE 2048

Net::HTTPProxyClient::HTTPProxyClient(NN<Net::SocketFactory> sockf, Bool noShutdown, UInt32 proxyIP, UInt16 proxyPort) : Net::HTTPMyClient(sockf, 0, CSTR_NULL, noShutdown)
{
	this->proxyIP = proxyIP;
	this->proxyPort = proxyPort;
	this->authBuff = 0;
	this->authBuffSize = 0;
}

Net::HTTPProxyClient::~HTTPProxyClient()
{
	if (this->authBuff)
	{
		MemFree(this->authBuff);
	}
}

Bool Net::HTTPProxyClient::Connect(Text::CStringNN url, Net::WebUtil::RequestMethod method, OptOut<Double> timeDNS, OptOut<Double> timeConn, Bool defHeaders)
{
	UTF8Char urltmp[256];
	UTF8Char svrname[256];
	UTF8Char host[256];
	UnsafeArray<UTF8Char> hostEnd;

	UOSInt i;
	const UTF8Char *ptr1;
	Text::PString ptrs[2];
	UnsafeArray<UTF8Char> cptr;
	NN<Net::TCPClient> cli;

	if (url.StartsWith(UTF8STRC("http://")))
	{
		ptr1 = &url.v[7];
		i = Text::StrIndexOfCharC(ptr1, url.leng - 7, '/');
		if (i != INVALID_INDEX)
		{
			MemCopyNO(urltmp, ptr1, i * sizeof(UTF8Char));
			urltmp[i] = 0;
		}
		else
		{
			i = url.leng - 7;
			MemCopyNO(urltmp, ptr1, i * sizeof(UTF8Char));
			urltmp[i] = 0;
		}
		cptr = Text::StrConcatC(host, UTF8STRC("Host: "));
		cptr = Text::StrConcatC(cptr, urltmp, i);
		hostEnd = Text::StrConcatC(cptr, UTF8STRC("\r\n"));
		i = Text::StrSplitP(ptrs, 2, {urltmp, i}, ':');
		if (i == 2)
		{
			Text::StrConcatC(svrname, ptrs[0].v, ptrs[0].leng);
		}
		else
		{
			Text::StrConcatC(svrname, ptrs[0].v, ptrs[0].leng);
		}
		this->clk.Start();
		Double t1;

		timeDNS.Set(0);
		this->svrAddr.addrType = Net::AddrType::IPv4;
		WriteNUInt32(this->svrAddr.addr, this->proxyIP);

		NEW_CLASSNN(cli, Net::TCPClient(sockf, this->proxyIP, this->proxyPort, 30000));
		t1 = this->clk.GetTimeDiff();
		timeConn.Set(t1);
#ifdef DEBUGSPEED
		if (t1 > 0.01)
		{
			printf("Time in connect: %lf\n", t1);
		}
#endif
		NN<Socket> soc;
		if (cli->IsConnectError() || !cli->GetSocket().SetTo(soc))
		{
			cli.Delete();
			this->writing = true;
			this->canWrite = false;
			return false;
		}
		else
		{
			this->cli = cli;
			this->sockf->SetLinger(soc, 0);
			i = url.leng;
			if ((i + 16) > BUFFSIZE)
			{
				MemFree(this->dataBuff);
				this->dataBuff = MemAlloc(UInt8, (i + 16));
			}
			if (method == Net::WebUtil::RequestMethod::HTTP_POST)
			{
				this->canWrite = true;
				this->writing = false;
				cptr = Text::StrConcatC(dataBuff, UTF8STRC("POST "));
			}
			else
			{
				cptr = Text::StrConcatC(dataBuff, UTF8STRC("GET "));
			}
			cptr = url.ConcatTo(cptr);
			cptr = Text::StrConcatC(cptr, UTF8STRC(" HTTP/1.1\r\n"));
			cli->Write(Data::ByteArrayR((UInt8*)dataBuff, (UOSInt)(cptr - dataBuff)));
			cli->Write(Data::ByteArrayR((UInt8*)host, (UOSInt)(hostEnd - host)));
			return true;
		}
	}
	else
	{
		timeDNS.Set(-1);
		timeConn.Set(-1);
		return false;
	}
}

Bool Net::HTTPProxyClient::SetAuthen(Net::HTTPProxyTCPClient::PasswordType pwdType, const UTF8Char *userName, const UTF8Char *password)
{
	if (pwdType == Net::HTTPProxyTCPClient::PWDT_BASIC)
	{
		UTF8Char userPwd[128];
		UTF8Char buff[512];
		UnsafeArray<UTF8Char> sptr2 = userPwd;
		UnsafeArray<UTF8Char> sptr;
		Text::Encoding enc(65001);
		sptr2 = Text::StrConcat(sptr2, userName);
		*sptr2++ = ':';
		sptr2 = Text::StrConcat(sptr2, password);

		sptr = Text::StrConcatC(buff, UTF8STRC("BASIC "));
		Crypto::Encrypt::Base64 b64;
		sptr = sptr + b64.Encrypt(userPwd, (UOSInt)(sptr2 - userPwd), sptr);
		*sptr = 0;
		this->AddHeaderC(CSTR("Proxy-Authorization"), {(const UTF8Char*)buff, (UOSInt)(sptr - buff)});
		return true;
	}
	return false;
}
