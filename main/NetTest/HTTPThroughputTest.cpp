#include "Stdafx.h"
#include "Core/Core.h"
#include "Net/HTTPClient.h"
#include "Net/OSSocketFactory.h"
#include "Net/SSLEngineFactory.h"
#include "Sync/Interlocked.h"
#include "Sync/Thread.h"
#include <stdio.h>

#define URL "http://127.0.0.1:8081"
#define THREADCNT 10
#define CONNCNT 10000
#define KACONN true
#define METHOD "GET"
#define POSTSIZE 1048576

UInt32 threadCurrCnt;
Int32 connLeft;
UInt32 connCnt;
UInt32 failCnt;
Manage::HiResClock *clk;
Double t;
Net::SocketFactory *sockf;
Net::SSLEngine *ssl;

struct ThreadStatus
{
	Bool threadRunning;
	Bool threadToStop;
	Sync::Event *evt;
};

UInt32 __stdcall ProcessThread(void *userObj)
{
	ThreadStatus *status = (ThreadStatus*)userObj;
	Net::HTTPClient *cli = 0;
//	UInt8 buff[2048];
	Text::CString url;
	Double timeDNS;
	Double timeConn;
	Double timeReq;
	Double timeResp;
	UInt8 buff[2048];
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	UInt32 cnt;
	Interlocked_IncrementU32(&threadCurrCnt);
	status->threadRunning = true;
	if (KACONN)
	{
		cli = Net::HTTPClient::CreateClient(sockf, ssl, 0, 0, true, false);
		while (!status->threadToStop)
		{
			url = {UTF8STRC(URL)};
			if (Interlocked_DecrementI32(&connLeft) < 0)
				break;
			if (cli->Connect(url.v, url.len, METHOD, &timeDNS, &timeConn, false))
			{
				cli->AddHeaderC(UTF8STRC("Connection"), UTF8STRC("keep-alive"));
				if (Text::StrEqualsC(UTF8STRC(METHOD), UTF8STRC("POST")))
				{
					i = POSTSIZE;
					sptr = Text::StrUOSInt(buff, i);
					cli->AddHeaderC(UTF8STRC("Content-Length"), buff, (UOSInt)(sptr - buff));
					while (i >= 2048)
					{
						j = cli->Write(buff, 2048);
						if (j <= 0)
						{
							break;
						}
						i -= j;
					}
					if (i > 0)
					{
						cli->Write(buff, i);
					}
				}
				cli->EndRequest(&timeReq, &timeResp);
				if (timeResp >= 0)
				{
					Interlocked_IncrementU32(&connCnt);
					if (timeResp > 0.5)
					{
						if (timeConn > 0.5)
						{
							i = 0;
						}
						else
						{
							i = 0;
						}
					}
					while (cli->Read(buff, 2048));
				}
				else
				{
					Interlocked_IncrementU32(&failCnt);
				}
				if (cli->IsError())
				{
					DEL_CLASS(cli);
					cli = Net::HTTPClient::CreateClient(sockf, ssl, 0, 0, true, Text::StrStartsWithC(url.v, url.len, UTF8STRC("https://")));
				}
			}
			else
			{
				DEL_CLASS(cli);
				cli = Net::HTTPClient::CreateClient(sockf, ssl, 0, 0, true, Text::StrStartsWithC(url.v, url.len, UTF8STRC("https://")));
				Interlocked_IncrementU32(&failCnt);
			}
		}
		DEL_CLASS(cli);
	}
	else
	{
		while (!status->threadToStop)
		{
			url = {UTF8STRC(URL)};
			if (Sync::Interlocked::Decrement(&connLeft) < 0)
				break;
			cli = Net::HTTPClient::CreateClient(sockf, ssl, 0, 0, true, Text::StrStartsWithC(url.v, url.len, UTF8STRC("https://")));
			if (cli->Connect(url.v, url.len, "GET", &timeDNS, &timeConn, false))
			{
				cli->AddHeaderC(UTF8STRC("Connection"), UTF8STRC("keep-alive"));
				cli->EndRequest(&timeReq, &timeResp);
				if (timeResp >= 0)
				{
					Sync::Interlocked::Increment(&connCnt);
				}
				else
				{
					Sync::Interlocked::Increment(&failCnt);
				}
			}
			else
			{
				Sync::Interlocked::Increment(&failCnt);
			}
			DEL_CLASS(cli);
		}
	}
	status->threadToStop = false;
	status->threadRunning = false;
	cnt = Interlocked_DecrementU32(&threadCurrCnt);
	if (cnt == 0)
	{
		t = clk->GetTimeDiff();
	}
	return 0;
}

Int32 MyMain(Core::IProgControl *progCtrl)
{
	threadCurrCnt = 0;
	connCnt = 0;
	failCnt = 0;
	connLeft = CONNCNT;
	t = 0;
	NEW_CLASS(clk, Manage::HiResClock());
	NEW_CLASS(sockf, Net::OSSocketFactory(true));
	ssl = Net::SSLEngineFactory::Create(sockf, true);
	ThreadStatus *threadStatus = MemAlloc(ThreadStatus, THREADCNT);
	clk->Start();
	UOSInt i = THREADCNT;
	while (i-- > 0)
	{
		threadStatus[i].threadRunning = false;
		threadStatus[i].threadToStop = false;
		NEW_CLASS(threadStatus[i].evt, Sync::Event(true, (const UTF8Char*)"threadStatus.evt"));
		Sync::Thread::Create(ProcessThread, &threadStatus[i]);
	}
	while (threadCurrCnt > 0 || connLeft > 0)
	{
		Sync::Thread::Sleep(10);
	}
	i = THREADCNT;
	while (i-- > 0)
	{
		DEL_CLASS(threadStatus[i].evt);
	}
	MemFree(threadStatus);

	printf("TotalConn = %d\r\n", connCnt);
	printf("FailCnt = %d\r\n", failCnt);
	printf("Time used = %lf\r\n", t);
	printf("Speed = %lf req/s\r\n", (connCnt / t));
	
	SDEL_CLASS(ssl);
	DEL_CLASS(sockf);
	DEL_CLASS(clk);
	return 0;
}
