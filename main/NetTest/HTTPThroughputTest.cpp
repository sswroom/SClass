#include "Stdafx.h"
#include "Core/Core.h"
#include "Math/Unit/Count.h"
#include "Net/HTTPClient.h"
#include "Net/OSSocketFactory.h"
#include "Net/SSLEngineFactory.h"
#include "Sync/Interlocked.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include <stdio.h>

#define URL "http://127.0.0.1:8080"
#define THREADCNT 16
#define CONNCNT 100000
#define KACONN true
#define METHOD Net::WebUtil::RequestMethod::HTTP_GET
#define POSTSIZE 1048576

Text::CString paramUrl;
UOSInt threadCnt;
UOSInt paramConnCnt;
Bool kaConn;
Net::WebUtil::RequestMethod reqMeth;
UOSInt postSize;

UInt32 threadCurrCnt;
Int32 connLeft;
Manage::HiResClock *clk;
Double t;
Net::SocketFactory *sockf;
Net::SSLEngine *ssl;

struct ThreadStatus
{
	Bool threadRunning;
	Bool threadToStop;
	Sync::Event *evt;
	UInt32 connCnt;
	UInt32 failCnt;
	Double totalRespTime;
	Double maxRespTime;
	UInt64 recvSize;
	UInt64 hdrSize;
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
	Double respT;
	UOSInt recvSize;
	{
		Sync::Event evt;
		status->evt = &evt;
		Interlocked_IncrementU32(&threadCurrCnt);
		status->threadRunning = true;
		Manage::HiResClock respClk;
		if (kaConn)
		{
			cli = Net::HTTPClient::CreateClient(sockf, ssl, CSTR_NULL, true, false);
			while (!status->threadToStop)
			{
				if (Interlocked_DecrementI32(&connLeft) < 0)
					break;
				respClk.Start();
				if (cli->Connect(paramUrl, METHOD, &timeDNS, &timeConn, false))
				{
					cli->AddHeaderC(CSTR("Connection"), CSTR("keep-alive"));
					if (reqMeth == Net::WebUtil::RequestMethod::HTTP_POST)
					{
						i = POSTSIZE;
						sptr = Text::StrUOSInt(buff, i);
						cli->AddHeaderC(CSTR("Content-Length"), {buff, (UOSInt)(sptr - buff)});
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
						status->connCnt++;
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
						while ((recvSize = cli->Read(buff, 2048)) > 0)
						{
							status->recvSize += recvSize;
						}
						respT = respClk.GetTimeDiff();
						status->hdrSize += cli->GetHdrLen();
						status->totalRespTime += respT;
						if (respT > status->maxRespTime)
						{
							status->maxRespTime = respT;
						}
					}
					else
					{
						status->failCnt++;
					}
					if (cli->IsError())
					{
						DEL_CLASS(cli);
						cli = Net::HTTPClient::CreateClient(sockf, ssl, CSTR_NULL, true, url.StartsWith(UTF8STRC("https://")));
					}
				}
				else
				{
					DEL_CLASS(cli);
					cli = Net::HTTPClient::CreateClient(sockf, ssl, CSTR_NULL, true, url.StartsWith(UTF8STRC("https://")));
					status->failCnt++;
				}
			}
			DEL_CLASS(cli);
		}
		else
		{
			while (!status->threadToStop)
			{
				url = CSTR(URL);
				if (Sync::Interlocked::Decrement(&connLeft) < 0)
					break;
				respClk.Start();
				cli = Net::HTTPClient::CreateClient(sockf, ssl, CSTR_NULL, true, url.StartsWith(UTF8STRC("https://")));
				if (cli->Connect(url, Net::WebUtil::RequestMethod::HTTP_GET, &timeDNS, &timeConn, false))
				{
					cli->AddHeaderC(CSTR("Connection"), CSTR("keep-alive"));
					cli->EndRequest(&timeReq, &timeResp);
					if (timeResp >= 0)
					{
						while ((recvSize = cli->Read(buff, 2048)) > 0)
						{
							status->recvSize += recvSize;
						}
						respT = respClk.GetTimeDiff();
						status->hdrSize += cli->GetHdrLen();
						status->connCnt++;
						status->totalRespTime += respT;
						if (respT > status->maxRespTime)
						{
							status->maxRespTime = respT;
						}
					}
					else
					{
						status->failCnt++;
					}
				}
				else
				{
					status->failCnt++;
				}
				DEL_CLASS(cli);
			}
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

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	paramUrl = CSTR(URL);
	threadCnt = THREADCNT;
	paramConnCnt = CONNCNT;
	kaConn = KACONN;
	reqMeth = METHOD;
	postSize = POSTSIZE;

	UTF8Char sbuff[256];
	UOSInt i;
	UOSInt cmdCnt;
	UTF8Char **args = progCtrl->GetCommandLines(progCtrl, &cmdCnt);
	Bool showHelp = false;

	i = 1;
	while (i < cmdCnt)
	{
		UOSInt cmdLen = Text::StrCharCnt(args[i]);
		if (args[i][0] == '-')
		{
			switch (args[i][1])
			{
			case 'h':
				showHelp = true;
				break;
			case 'c':
			case 't':
				threadCnt = Text::StrToUOSInt(&args[i][2]);
				break;
			case 'r':
				paramConnCnt = Text::StrToUOSInt(&args[i][2]);
				break;
			case 'k':
				kaConn = (args[i][2] != 'c');
				break;
			default:
				showHelp = true;
				break;
			}
		}
		else if (Text::StrStartsWithC(args[i], cmdLen, UTF8STRC("http://")) || Text::StrStartsWithC(args[i], cmdLen, UTF8STRC("https://")))
		{
			paramUrl.v = args[i];
			paramUrl.leng = cmdLen;
		}
		i++;
	}
	if (showHelp)
	{
		printf("Usage: HTTPThroughputTest <Options> <url>\r\n");
		printf("  Options:\r\n");
		printf("    -h     This help\r\n");
		printf("    -tN    Number of concurrent connection/thread\r\n");
		printf("    -cN    Number of concurrent connection/thread\r\n");
		printf("    -rN    Number of request to test\r\n");
		printf("    -kc    Connection Type = close\r\n");
		printf("    -k     Connection Type = keep-alive\r\n");
		return 0;
	}

	threadCurrCnt = 0;
	connLeft = (Int32)paramConnCnt;
	t = 0;

	Manage::HiResClock localClk;
	Net::OSSocketFactory localSockf(true);
	clk = &localClk;
	sockf = &localSockf;
	ssl = Net::SSLEngineFactory::Create(sockf, true);
	ThreadStatus *threadStatus = MemAlloc(ThreadStatus, threadCnt);
	clk->Start();
	i = threadCnt;
	while (i-- > 0)
	{
		threadStatus[i].threadRunning = false;
		threadStatus[i].threadToStop = false;
		threadStatus[i].connCnt = 0;
		threadStatus[i].failCnt = 0;
		threadStatus[i].totalRespTime = 0;
		threadStatus[i].maxRespTime = 0;
		threadStatus[i].recvSize = 0;
		threadStatus[i].hdrSize = 0;
		Sync::ThreadUtil::Create(ProcessThread, &threadStatus[i]);
	}
	while (threadCurrCnt > 0 || connLeft > 0)
	{
		Sync::SimpleThread::Sleep(10);
	}
	UInt64 connCnt = 0;
	UInt64 failCnt = 0;
	UInt64 recvSize = 0;
	UInt64 hdrSize = 0;
	Double totalRespTime = 0;
	Double maxRespTime = 0;
	i = threadCnt;
	while (i-- > 0)
	{
		connCnt += threadStatus[i].connCnt;
		failCnt += threadStatus[i].failCnt;
		recvSize += threadStatus[i].recvSize;
		hdrSize += threadStatus[i].hdrSize;
		totalRespTime += threadStatus[i].totalRespTime;
		if (threadStatus[i].maxRespTime > maxRespTime)
		{
			maxRespTime = threadStatus[i].maxRespTime;
		}
	}
	MemFree(threadStatus);

	if (kaConn)
	{
		printf("Connection Type: keep-alive\r\n");
	}
	else
	{
		printf("Connection Type: close\r\n");
	}
	printf("URL: %s\r\n", paramUrl.v);
	printf("Thread Cnt = %d\r\n", (UInt32)threadCnt);
	printf("Total Request = %d\r\n", (UInt32)paramConnCnt);
	printf("Succ Count = %lld\r\n", connCnt);
	printf("Fail Count = %lld\r\n", failCnt);
	printf("Time used = %lf\r\n", t);
	Text::StrConcatC(Math::Unit::Count::WellFormat(sbuff, (Double)hdrSize), UTF8STRC("B"));
	printf("Total header Size = %s\r\n", sbuff);
	Text::StrConcatC(Math::Unit::Count::WellFormat(sbuff, (Double)recvSize), UTF8STRC("B"));
	printf("Total data Size = %s\r\n", sbuff);
	printf("Avg Resp Time = %lf s\r\n", totalRespTime / (Double)connCnt);
	printf("Max Resp Time = %lf s\r\n", maxRespTime);
	Math::Unit::Count::WellFormat(sbuff, ((Double)connCnt / t));
	printf("Request/s = %s\r\n", sbuff);
	Text::StrConcatC(Math::Unit::Count::WellFormat(sbuff, ((Double)(recvSize + hdrSize) / t)), UTF8STRC("B/s"));
	printf("Transfer/s = %s\r\n", sbuff);
	
	SDEL_CLASS(ssl);
	return 0;
}
