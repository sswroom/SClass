#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/DateTime.h"
#include "Data/ArrayList.h"
#include "Data/ArrayListInt32.h"
#include "IO/FileStream.h"
#include "IO/LogTool.h"
#include "IO/Stream.h"
#include "Manage/HiResClock.h"
#include "Net/SocketFactory.h"
#include "Net/TCPServer.h"
#include "Sync/Mutex.h"
#include "Sync/Event.h"
#include "Sync/Thread.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/UTF8Writer.h"
#include <wchar.h>

void Net::TCPServer::AddLogMsg(const UTF8Char *msg, IO::ILogHandler::LogLevel logLev)
{
	if (log)
	{
		if (logPrefix)
		{
			UTF8Char buff[1024];
			UTF8Char *str = Text::StrConcat(buff, this->logPrefix);
			str = Text::StrConcat(str, msg);
			log->LogMessage(buff, logLev);
		}
		else
		{
			log->LogMessage(msg, logLev);
		}
	}
}

UInt32 __stdcall Net::TCPServer::Svrv4Subthread(void *o)
{
	SubthreadStatus *status = (SubthreadStatus*)o;
	UTF8Char buff[1024];
	UTF8Char *str;
	status->threadRunning = true;
	status->threadEvt->Set();
	while (!status->toStop)
	{
		UInt32 *s;
		s = status->me->socf->SocketAccept(status->me->svrSocv4);
		if (status->me->socf->SocketIsInvalid(s))
		{
			str = Text::StrConcat(buff, (const UTF8Char*)"Client connect error: ");
			str = Text::StrInt32(str, status->me->socf->SocketGetLastError());
			status->me->AddLogMsg(buff, IO::ILogHandler::LOG_LEVEL_ERR_DETAIL);
		}
		else
		{
			status->me->socs->Put((UInt32*)s);
			status->me->socsEvt->Set();
		}
	}
	status->threadRunning = false;
	status->threadEvt->Set();
	return 0;
}

UInt32 __stdcall Net::TCPServer::Svrv4Thread(void *o)
{
	Net::TCPServer *svr = (Net::TCPServer*)o;
	UTF8Char buff[1024];
	UTF8Char *str;
	OSInt sthreadCnt = 1;
	OSInt i;
	Bool found;
	SubthreadStatus *sthreads = 0;
	Sync::Event *threadEvt = 0;

	Sync::Thread::SetPriority(Sync::Thread::TP_HIGHEST);
	svr->svrSocv4 = svr->socf->CreateTCPSocketv4();
	if (!svr->socf->SocketBindv4(svr->svrSocv4, 0, svr->port))
	{
		str = Text::StrConcat(buff, (const UTF8Char*)"Cannot bind to the v4 port: ");
		str = Text::StrInt32(str, svr->port);
		svr->AddLogMsg(buff, IO::ILogHandler::LOG_LEVEL_ERROR);
		svr->socf->DestroySocket(svr->svrSocv4);
		svr->svrSocv4 = 0;
		svr->errorv4 = true;
		return 0;
	}
	if (!svr->socf->SocketListen(svr->svrSocv4))
	{
		str = Text::StrConcat(buff, (const UTF8Char*)"Cannot start listening the v4 port: ");
		str = Text::StrInt32(str, svr->port);
		svr->AddLogMsg(buff, IO::ILogHandler::LOG_LEVEL_ERROR);
		svr->socf->DestroySocket(svr->svrSocv4);
		svr->svrSocv4 = 0;
		svr->errorv4 = true;
		return 0;
	}

	svr->threadRunning |= 1;
	str = Text::StrConcat(buff, (const UTF8Char*)"Start listening to v4 port ");
	str = Text::StrInt32(str, svr->port);
	svr->AddLogMsg(buff, IO::ILogHandler::LOG_LEVEL_ACTION);

	if (sthreadCnt > 0)
	{
		sthreads = MemAlloc(SubthreadStatus, sthreadCnt);
		NEW_CLASS(threadEvt, Sync::Event(true, (const UTF8Char*)"Net.TCPServer.threadEvt"));
		i = sthreadCnt;
		while (i-- > 0)
		{
			sthreads[i].me = svr;
			sthreads[i].threadRunning = false;
			sthreads[i].toStop = false;
			sthreads[i].threadEvt = threadEvt;
			Sync::Thread::Create(Svrv4Subthread, &sthreads[i]);
		}
		found = true;
		while (found)
		{
			found = false;
			i = sthreadCnt;
			while (i-- > 0)
			{
				if (!sthreads[i].threadRunning)
				{
					found = true;
					break;
				}
			}
			if (!found)
				break;
			threadEvt->Wait(100);
		}
	}
	while (!svr->toStop)
	{
		UInt32 *s;
		s = svr->socf->SocketAccept(svr->svrSocv4);
		if (svr->socf->SocketIsInvalid(s))
		{
			str = Text::StrConcat(buff, (const UTF8Char*)"Client connect error: ");
			str = Text::StrInt32(str, svr->socf->SocketGetLastError());
			svr->AddLogMsg(buff, IO::ILogHandler::LOG_LEVEL_ERR_DETAIL);
		}
		else
		{
			svr->socs->Put((UInt32*)s);
			svr->socsEvt->Set();

/*			str = Text::StrConcat(buff, L"Client connected: ");
			str = svr->socf->GetRemoteName(str, (UInt32*)s);
			svr->AddLogMsg(buff, IO::ILogHandler::LOG_LEVEL_ACTION);
			svr->hdlr((UInt32*)s, svr->userObj);*/
		}
	}
	if (sthreadCnt > 0)
	{
		i = sthreadCnt;
		while (i-- > 0)
		{
			sthreads[i].toStop = true;
		}
	}
	svr->socf->DestroySocket(svr->svrSocv4);
	if (sthreadCnt > 0)
	{
		found = true;
		while (true)
		{
			found = false;
			i = sthreadCnt;
			while (i-- > 0)
			{
				if (sthreads[i].threadRunning)
				{
					found = true;
					break;
				}
			}
			if (!found)
			{
				break;
			}
			threadEvt->Wait();
		}
		MemFree(sthreads);
		DEL_CLASS(threadEvt);
	}
	str = Text::StrConcat(buff, (const UTF8Char*)"End listening on v4 port ");
	str = Text::StrInt32(str, svr->port);
	svr->AddLogMsg(buff, IO::ILogHandler::LOG_LEVEL_ACTION);
	svr->threadRunning &= ~1;
	return 0;
}

UInt32 __stdcall Net::TCPServer::Svrv6Subthread(void *o)
{
	SubthreadStatus *status = (SubthreadStatus*)o;
	UTF8Char buff[1024];
	UTF8Char *str;
	status->threadRunning = true;
	status->threadEvt->Set();
	while (!status->toStop)
	{
		UInt32 *s;
		s = status->me->socf->SocketAccept(status->me->svrSocv6);
		if (status->me->socf->SocketIsInvalid(s))
		{
			str = Text::StrConcat(buff, (const UTF8Char*)"Client connect error: ");
			str = Text::StrInt32(str, status->me->socf->SocketGetLastError());
			status->me->AddLogMsg(buff, IO::ILogHandler::LOG_LEVEL_ERR_DETAIL);
		}
		else
		{
			status->me->socs->Put((UInt32*)s);
			status->me->socsEvt->Set();
		}
	}
	status->threadRunning = false;
	status->threadEvt->Set();
	return 0;
}

UInt32 __stdcall Net::TCPServer::Svrv6Thread(void *o)
{
	Net::TCPServer *svr = (Net::TCPServer*)o;
	UTF8Char buff[1024];
	UTF8Char *str;
	OSInt sthreadCnt = 1;
	OSInt i;
	Bool found;
	SubthreadStatus *sthreads = 0;
	Sync::Event *threadEvt = 0;

	Sync::Thread::SetPriority(Sync::Thread::TP_HIGHEST);
	svr->svrSocv6 = svr->socf->CreateTCPSocketv6();
	Net::SocketUtil::AddressInfo addrAny;
	Net::SocketUtil::SetAddrAnyV6(&addrAny);
	if (!svr->socf->SocketBind(svr->svrSocv6, &addrAny, svr->port))
	{
		str = Text::StrConcat(buff, (const UTF8Char*)"Cannot bind to the v6 port: ");
		str = Text::StrInt32(str, svr->port);
		svr->AddLogMsg(buff, IO::ILogHandler::LOG_LEVEL_ERROR);
		svr->socf->DestroySocket(svr->svrSocv6);
		svr->svrSocv6 = 0;
		svr->errorv6 = true;
		return 0;
	}
	if (!svr->socf->SocketListen(svr->svrSocv6))
	{
		str = Text::StrConcat(buff, (const UTF8Char*)"Cannot start listening the v6 port: ");
		str = Text::StrInt32(str, svr->port);
		svr->AddLogMsg(buff, IO::ILogHandler::LOG_LEVEL_ERROR);
		svr->socf->DestroySocket(svr->svrSocv6);
		svr->svrSocv6 = 0;
		svr->errorv6 = true;
		return 0;
	}

	svr->threadRunning |= 4;
	str = Text::StrConcat(buff, (const UTF8Char*)"Start listening to v6 port ");
	str = Text::StrInt32(str, svr->port);
	svr->AddLogMsg(buff, IO::ILogHandler::LOG_LEVEL_ACTION);

	if (sthreadCnt > 0)
	{
		sthreads = MemAlloc(SubthreadStatus, sthreadCnt);
		NEW_CLASS(threadEvt, Sync::Event(true, (const UTF8Char*)"Net.TCPServer.threadEvt"));
		i = sthreadCnt;
		while (i-- > 0)
		{
			sthreads[i].me = svr;
			sthreads[i].threadRunning = false;
			sthreads[i].toStop = false;
			sthreads[i].threadEvt = threadEvt;
			Sync::Thread::Create(Svrv6Subthread, &sthreads[i]);
		}
		found = true;
		while (found)
		{
			found = false;
			i = sthreadCnt;
			while (i-- > 0)
			{
				if (!sthreads[i].threadRunning)
				{
					found = true;
					break;
				}
			}
			if (!found)
				break;
			threadEvt->Wait(100);
		}
	}
	while (!svr->toStop)
	{
		UInt32 *s;
		s = svr->socf->SocketAccept(svr->svrSocv6);
		if (svr->socf->SocketIsInvalid(s))
		{
			str = Text::StrConcat(buff, (const UTF8Char*)"Client connect error: ");
			str = Text::StrInt32(str, svr->socf->SocketGetLastError());
			svr->AddLogMsg(buff, IO::ILogHandler::LOG_LEVEL_ERR_DETAIL);
		}
		else
		{
			svr->socs->Put((UInt32*)s);
			svr->socsEvt->Set();

/*			str = Text::StrConcat(buff, L"Client connected: ");
			str = svr->socf->GetRemoteName(str, (UInt32*)s);
			svr->AddLogMsg(buff, IO::ILogHandler::LOG_LEVEL_ACTION);
			svr->hdlr((UInt32*)s, svr->userObj);*/
		}
	}
	if (sthreadCnt > 0)
	{
		i = sthreadCnt;
		while (i-- > 0)
		{
			sthreads[i].toStop = true;
		}
	}
	svr->socf->DestroySocket(svr->svrSocv6);
	if (sthreadCnt > 0)
	{
		found = true;
		while (true)
		{
			found = false;
			i = sthreadCnt;
			while (i-- > 0)
			{
				if (sthreads[i].threadRunning)
				{
					found = true;
					break;
				}
			}
			if (!found)
			{
				break;
			}
			threadEvt->Wait();
		}
		MemFree(sthreads);
		DEL_CLASS(threadEvt);
	}
	str = Text::StrConcat(buff, (const UTF8Char*)"End listening on v6 port ");
	str = Text::StrInt32(str, svr->port);
	svr->AddLogMsg(buff, IO::ILogHandler::LOG_LEVEL_ACTION);
	svr->threadRunning &= ~4;
	return 0;
}

UInt32 __stdcall Net::TCPServer::SvrThread2(void *o)
{
	Net::TCPServer *svr = (Net::TCPServer*)o;
	UTF8Char buff[256];
	UTF8Char *str;

	svr->threadRunning |= 2;

	while (!svr->toStop)
	{
		while (svr->socs->HasItems())
		{
			UInt32 *s = (UInt32*)svr->socs->Get();
			str = Text::StrConcat(buff, (const UTF8Char*)"Client connected: ");
			str = svr->socf->GetRemoteName(str, (UInt32*)s);
			svr->AddLogMsg(buff, IO::ILogHandler::LOG_LEVEL_ACTION);
			svr->hdlr(s, svr->userObj);
		}
		svr->socsEvt->Wait(100);
	}
	svr->threadRunning &= ~2;
	return 0;
}

Net::TCPServer::TCPServer(SocketFactory *socf, UInt16 port, IO::LogTool *log, TCPServerConn hdlr, void *userObj, const UTF8Char *logPrefix)
{
	this->socf = socf;
	this->toStop = false;
	this->errorv4 = false;
	this->errorv6 = false;
	this->port = port;
	this->log = log;
	this->svrSocv4 = 0;
	this->svrSocv6 = 0;
	if (logPrefix)
	{
		this->logPrefix = Text::StrCopyNew(logPrefix);
	}
	else
	{
		this->logPrefix = 0;
	}
	this->hdlr = hdlr;
	this->userObj = userObj;
	this->threadRunning = 0;
	NEW_CLASS(this->socs, Data::LinkedList());
	NEW_CLASS(this->socsEvt, Sync::Event(true, (const UTF8Char*)"Net.TCPServer.socsEvt"));

	Sync::Thread::Create(Svrv4Thread, this);
	while (true)
	{
		if (threadRunning & 1 || errorv4)
			break;
		Sync::Thread::Sleep(10);
	}
	Sync::Thread::Create(Svrv6Thread, this);
	Sync::Thread::Create(SvrThread2, this);
	while (true)
	{
		if (threadRunning & 1 || errorv4)
			if (threadRunning & 2)
				if (threadRunning & 4 || errorv6)
					break;
		Sync::Thread::Sleep(10);
	}
}

Net::TCPServer::~TCPServer()
{
	this->Close();
	DEL_CLASS(this->socs);
	while (threadRunning)
	{
		Sync::Thread::Sleep(10);
	}
	if (this->logPrefix)
		Text::StrDelNew(this->logPrefix);
	DEL_CLASS(this->socsEvt);
}

void Net::TCPServer::Close()
{
	if (!toStop)
	{
		toStop = true;
		if (this->svrSocv4)
		{
			socf->DestroySocket(this->svrSocv4);
		}
		if (this->svrSocv6)
		{
			socf->DestroySocket(this->svrSocv6);
		}
		this->socsEvt->Set();
	}
}

Bool Net::TCPServer::IsV4Error()
{
	return errorv4;
}

Bool Net::TCPServer::IsV6Error()
{
	return errorv6;
}
