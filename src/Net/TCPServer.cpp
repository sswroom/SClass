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
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/UTF8Writer.h"

void Net::TCPServer::AddLogMsgC(const UTF8Char *msg, UOSInt msgLen, IO::LogHandler::LogLevel logLev)
{
	if (this->log->HasHandler())
	{
		NotNullPtr<Text::String> s;
		if (logPrefix.SetTo(s))
		{
			UTF8Char buff[1024];
			UTF8Char *str = s->ConcatTo(buff);
			str = Text::StrConcatC(str, msg, msgLen);
			log->LogMessage(CSTRP(buff, str), logLev);
		}
		else
		{
			log->LogMessage({msg, msgLen}, logLev);
		}
	}
}

UInt32 __stdcall Net::TCPServer::Svrv4Subthread(void *o)
{
	SubthreadStatus *status = (SubthreadStatus*)o;
	Sync::ThreadUtil::SetName(CSTR("TCPSvrv4Sub"));
	status->threadRunning = true;
	status->threadEvt->Set();
	while (!status->toStop)
	{
		status->me->AcceptSocket(status->me->svrSocv4);
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
	UOSInt sthreadCnt = 0;
	UOSInt i;
	Bool found;
	SubthreadStatus *sthreads = 0;
	Sync::Event *threadEvt = 0;

	Sync::ThreadUtil::SetPriority(Sync::ThreadUtil::TP_HIGHEST);
	Sync::ThreadUtil::SetName(CSTR("TCPSvrv4"));
	svr->threadRunning |= 1;
	str = Text::StrConcatC(buff, UTF8STRC("Start listening to v4 port "));
	str = Text::StrInt32(str, svr->port);
	svr->AddLogMsgC(buff, (UOSInt)(str - buff), IO::LogHandler::LogLevel::Action);

	if (sthreadCnt > 0)
	{
		sthreads = MemAlloc(SubthreadStatus, sthreadCnt);
		NEW_CLASS(threadEvt, Sync::Event(true));
		i = sthreadCnt;
		while (i-- > 0)
		{
			sthreads[i].me = svr;
			sthreads[i].threadRunning = false;
			sthreads[i].toStop = false;
			sthreads[i].threadEvt = threadEvt;
			Sync::ThreadUtil::Create(Svrv4Subthread, &sthreads[i]);
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
		svr->AcceptSocket(svr->svrSocv4);
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
	str = Text::StrConcatC(buff, UTF8STRC("End listening on v4 port "));
	str = Text::StrInt32(str, svr->port);
	svr->AddLogMsgC(buff, (UOSInt)(str - buff), IO::LogHandler::LogLevel::Action);
	svr->threadRunning &= ~1;
	return 0;
}

UInt32 __stdcall Net::TCPServer::Svrv6Subthread(void *o)
{
	SubthreadStatus *status = (SubthreadStatus*)o;
	Sync::ThreadUtil::SetName(CSTR("TCPSvrv6Sub"));
	status->threadRunning = true;
	status->threadEvt->Set();
	while (!status->toStop)
	{
		status->me->AcceptSocket(status->me->svrSocv6);
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
	UOSInt sthreadCnt = 0;
	UOSInt i;
	Bool found;
	SubthreadStatus *sthreads = 0;
	Sync::Event *threadEvt = 0;

	Sync::ThreadUtil::SetPriority(Sync::ThreadUtil::TP_HIGHEST);
	Sync::ThreadUtil::SetName(CSTR("TCPSvrv6"));
	svr->threadRunning |= 4;
	str = Text::StrConcatC(buff, UTF8STRC("Start listening to v6 port "));
	str = Text::StrInt32(str, svr->port);
	svr->AddLogMsgC(buff, (UOSInt)(str - buff), IO::LogHandler::LogLevel::Action);

	if (sthreadCnt > 0)
	{
		sthreads = MemAlloc(SubthreadStatus, sthreadCnt);
		NEW_CLASS(threadEvt, Sync::Event(true));
		i = sthreadCnt;
		while (i-- > 0)
		{
			sthreads[i].me = svr;
			sthreads[i].threadRunning = false;
			sthreads[i].toStop = false;
			sthreads[i].threadEvt = threadEvt;
			Sync::ThreadUtil::Create(Svrv6Subthread, &sthreads[i]);
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
		svr->AcceptSocket(svr->svrSocv6);
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
	str = Text::StrConcatC(buff, UTF8STRC("End listening on v6 port "));
	str = Text::StrInt32(str, svr->port);
	svr->AddLogMsgC(buff, (UOSInt)(str - buff), IO::LogHandler::LogLevel::Action);
	svr->threadRunning &= ~4;
	return 0;
}

UInt32 __stdcall Net::TCPServer::WorkerThread(void *o)
{
	Net::TCPServer *svr = (Net::TCPServer*)o;
	UTF8Char buff[256];
	UTF8Char *str;

	Sync::ThreadUtil::SetName(CSTR("TCPSvrWork"));
	svr->threadRunning |= 2;

	while (!svr->toStop)
	{
		while (svr->socs.HasItems())
		{
			Socket *s = (Socket*)svr->socs.Get();
			str = Text::StrConcatC(buff, UTF8STRC("Client connected: "));
			str = svr->socf->GetRemoteName(str, s);
			svr->AddLogMsgC(buff, (UOSInt)(str - buff), IO::LogHandler::LogLevel::Action);
			svr->hdlr(s, svr->userObj);
		}
		svr->socsEvt.Wait(100);
	}
	svr->threadRunning &= ~2;
	return 0;
}

void Net::TCPServer::AcceptSocket(Socket *svrSoc)
{
	UTF8Char sbuff[128];
	UTF8Char *sptr;
	Socket *s;
	s = this->socf->SocketAccept(svrSoc);
	if (this->socf->SocketIsInvalid(s))
	{
		Int32 errCode = this->socf->SocketGetLastError();
		sptr = Text::StrConcatC(sbuff, UTF8STRC("Client connect error: "));
		sptr = Text::StrInt32(sptr, errCode);
		this->AddLogMsgC(sbuff, (UOSInt)(sptr - sbuff), IO::LogHandler::LogLevel::ErrorDetail);
		if (errCode == 24) // too many opened files
		{
			Sync::SimpleThread::Sleep(2000);
		}
	}
	else
	{
		if (this->socs.GetLastNoRemove() == s)
		{
		}
		else
		{
			this->socs.Put(s);
			this->socsEvt.Set();
		}
/*		str = Text::StrConcatC(buff, UTF8STRC("Client connected: ");
		str = this->socf->GetRemoteName(str, (UInt32*)s);
		this->AddLogMsgC(buff, (UOSInt)(str - buff), IO::LogHandler::LogLevel::Action);
		this->hdlr((UInt32*)s, this->userObj);*/
	}	
}
Net::TCPServer::TCPServer(NotNullPtr<SocketFactory> socf, UInt16 port, NotNullPtr<IO::LogTool> log, TCPServerConn hdlr, void *userObj, Text::CString logPrefix, Bool autoStart)
{
	UTF8Char buff[1024];
	UTF8Char *str;
	this->socf = socf;
	this->toStop = false;
	this->errorv4 = false;
	this->errorv6 = false;
	this->port = port;
	this->log = log;
	this->svrSocv4 = 0;
	this->svrSocv6 = 0;
	this->logPrefix = Text::String::NewOrNull(logPrefix);
	this->hdlr = hdlr;
	this->userObj = userObj;
	this->threadRunning = 0;

	this->svrSocv4 = this->socf->CreateTCPSocketv4();
	if (!this->socf->SocketBindv4(this->svrSocv4, 0, this->port))
	{
		str = Text::StrConcatC(buff, UTF8STRC("Cannot bind to the v4 port: "));
		str = Text::StrInt32(str, this->port);
		this->AddLogMsgC(buff, (UOSInt)(str - buff), IO::LogHandler::LogLevel::Error);
		this->socf->DestroySocket(this->svrSocv4);
		this->svrSocv4 = 0;
		this->errorv4 = true;
	}
	else if (!this->socf->SocketListen(this->svrSocv4))
	{
		str = Text::StrConcatC(buff, UTF8STRC("Cannot start listening the v4 port: "));
		str = Text::StrInt32(str, this->port);
		this->AddLogMsgC(buff, (UOSInt)(str - buff), IO::LogHandler::LogLevel::Error);
		this->socf->DestroySocket(this->svrSocv4);
		this->svrSocv4 = 0;
		this->errorv4 = true;
	}

	this->svrSocv6 = this->socf->CreateTCPSocketv6();
	Net::SocketUtil::AddressInfo addrAny;
	Net::SocketUtil::SetAddrAnyV6(addrAny);
	if (!this->socf->SocketBind(this->svrSocv6, &addrAny, this->port))
	{
		str = Text::StrConcatC(buff, UTF8STRC("Cannot bind to the v6 port: "));
		str = Text::StrInt32(str, this->port);
		this->AddLogMsgC(buff, (UOSInt)(str - buff), IO::LogHandler::LogLevel::Error);
		this->socf->DestroySocket(this->svrSocv6);
		this->svrSocv6 = 0;
		this->errorv6 = true;
	}
	else if (!this->socf->SocketListen(this->svrSocv6))
	{
		str = Text::StrConcatC(buff, UTF8STRC("Cannot start listening the v6 port: "));
		str = Text::StrInt32(str, this->port);
		this->AddLogMsgC(buff, (UOSInt)(str - buff), IO::LogHandler::LogLevel::Error);
		this->socf->DestroySocket(this->svrSocv6);
		this->svrSocv6 = 0;
		this->errorv6 = true;
	}

	if (autoStart)
	{
		this->Start();
	}
}

Net::TCPServer::~TCPServer()
{
	this->Close();
	while (threadRunning)
	{
		Sync::SimpleThread::Sleep(10);
	}
	OPTSTR_DEL(this->logPrefix);
}

Bool Net::TCPServer::Start()
{
	if (this->errorv4)
		return false;
	if (this->threadRunning & 1)
		return true;
	Sync::ThreadUtil::Create(Svrv4Thread, this);
	while (true)
	{
		if (threadRunning & 1 || errorv4)
			break;
		Sync::SimpleThread::Sleep(10);
	}
	if (!this->errorv6)
	{
		Sync::ThreadUtil::Create(Svrv6Thread, this);
	}
	Sync::ThreadUtil::Create(WorkerThread, this);
	while (true)
	{
		if (threadRunning & 1 || errorv4)
			if (threadRunning & 2)
				if (threadRunning & 4 || errorv6)
					break;
		Sync::SimpleThread::Sleep(10);
	}
	return threadRunning & 1;
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
		this->socsEvt.Set();
		this->errorv4 = true;
		this->errorv6 = true;
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
