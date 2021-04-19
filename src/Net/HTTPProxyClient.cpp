#include "Stdafx.h"

#include "MyMemory.h"
#include "Crypto/Encrypt/Base64.h"
#include "Data/ArrayList.h"
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

Net::HTTPProxyClient::HTTPProxyClient(Net::SocketFactory *sockf, Bool noShutdown, UInt32 proxyIP, UInt16 proxyPort) : Net::HTTPMyClient(sockf, 0, noShutdown)
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

Bool Net::HTTPProxyClient::Connect(const UTF8Char *url, const Char *method, Double *timeDNS, Double *timeConn)
{
	UTF8Char urltmp[256];
	UTF8Char svrname[256];
	UTF8Char host[256];

	OSInt si;
	UOSInt ui;
	const UTF8Char *ptr1;
	UTF8Char *ptrs[2];
	UTF8Char *cptr;

	if (Text::StrStartsWith(url, (const UTF8Char*)"http://"))
	{
		ptr1 = &url[7];
		si = Text::StrIndexOf(ptr1, '/');
		if (si >= 0)
		{
			MemCopyNO(urltmp, ptr1, (UOSInt)si * sizeof(UTF8Char));
			urltmp[si] = 0;
		}
		else
		{
			ui = Text::StrCharCnt(ptr1);
			MemCopyNO(urltmp, ptr1, ui * sizeof(UTF8Char));
			urltmp[ui] = 0;
		}
		cptr = Text::StrConcat(host, (const UTF8Char*)"Host: ");
		cptr = Text::StrConcat(cptr, urltmp);
		cptr = Text::StrConcat(cptr, (const UTF8Char*)"\r\n");
		ui = Text::StrSplit(ptrs, 2, urltmp, ':');
		if (ui == 2)
		{
			Text::StrConcat(svrname, ptrs[0]);
		}
		else
		{
			Text::StrConcat(svrname, ptrs[0]);
		}
		this->clk->Start();
		Double t1;

		if (timeDNS)
		{
			*timeDNS = 0;
		}
		this->svrAddr.addrType = Net::SocketUtil::AT_IPV4;
		*(Int32*)this->svrAddr.addr = this->proxyIP;

		NEW_CLASS(cli, Net::TCPClient(sockf, this->proxyIP, this->proxyPort));
		t1 = clk->GetTimeDiff();
		if (timeConn)
		{
			*timeConn = t1;
		}
#ifdef DEBUGSPEED
		if (t1 > 0.01)
		{
			printf("Time in connect: %lf\n", t1);
		}
#endif
		if (cli->IsConnectError())
		{
			DEL_CLASS(cli);
			cli = 0;

			this->writing = true;
			this->canWrite = false;
			return false;
		}
		else
		{
			this->sockf->SetLinger(cli->GetSocket(), 0);
			ui = Text::StrCharCnt(url);
			if ((ui + 16) > BUFFSIZE)
			{
				MemFree(this->dataBuff);
				this->dataBuff = MemAlloc(UInt8, (ui + 16));
			}
			if (method)
			{
				if (Text::StrCompareICase(method, "POST") == 0)
				{
					this->canWrite = true;
					this->writing = false;
					cptr = Text::StrConcat(dataBuff, (const UTF8Char*)"POST ");
				}
				else
				{
					cptr = Text::StrConcat(dataBuff, (const UTF8Char*)"GET ");
				}
			}
			else
			{
				cptr = Text::StrConcat(dataBuff, (const UTF8Char*)"GET ");
			}
			cptr = Text::StrConcat(cptr, url);
			cptr = Text::StrConcat(cptr, (const UTF8Char*)" HTTP/1.1\r\n");
			cli->Write((UInt8*)dataBuff, cptr - dataBuff);
			cli->Write((UInt8*)host, Text::StrCharCnt(host));
			return true;
		}
	}
	else
	{
		if (timeDNS)
		{
			*timeDNS = -1;
		}
		if (timeConn)
		{
			*timeConn = -1;
		}
		return false;
	}
}

Bool Net::HTTPProxyClient::SetAuthen(Net::HTTPProxyTCPClient::PasswordType pwdType, const UTF8Char *userName, const UTF8Char *password)
{
	if (pwdType == Net::HTTPProxyTCPClient::PWDT_BASIC)
	{
		UTF8Char userPwd[128];
		UTF8Char buff[512];
		UTF8Char *sptr2 = userPwd;
		UTF8Char *sptr;
		Text::Encoding enc(65001);
		sptr2 = Text::StrConcat(sptr2, userName);
		*sptr2++ = ':';
		sptr2 = Text::StrConcat(sptr2, password);

		sptr = Text::StrConcat(buff, (const UTF8Char*)"BASIC ");
		Crypto::Encrypt::Base64 b64;
		sptr = buff + b64.Encrypt(userPwd, sptr2 - userPwd, sptr, 0);
		*sptr = 0;
		this->AddHeader((const UTF8Char*)"Proxy-Authorization", (const UTF8Char*)buff);
		return true;
	}
	return false;
}
