#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/DateTime.h"
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

void Net::TCPServer::AddLogMsgC(Text::CStringNN msg, IO::LogHandler::LogLevel logLev)
{
	if (this->log->HasHandler())
	{
		NN<Text::String> s;
		if (logPrefix.SetTo(s))
		{
			UTF8Char buff[1024];
			UnsafeArray<UTF8Char> str = s->ConcatTo(buff);
			str = msg.ConcatTo(str);
			log->LogMessage(CSTRP(buff, str), logLev);
		}
		else
		{
			log->LogMessage(msg, logLev);
		}
	}
}

UInt32 __stdcall Net::TCPServer::Svrv4Subthread(AnyType o)
{
	NN<SubthreadStatus> status = o.GetNN<SubthreadStatus>();
	NN<Socket> soc;
	Sync::ThreadUtil::SetName(CSTR("TCPSvrv4Sub"));
	status->threadRunning = true;
	status->threadEvt->Set();
	if (status->me->svrSocv4.SetTo(soc))
	{
		while (!status->toStop)
		{
			status->me->AcceptSocket(soc);
		}
	}
	status->threadRunning = false;
	status->threadEvt->Set();
	return 0;
}

UInt32 __stdcall Net::TCPServer::Svrv4Thread(AnyType o)
{
	NN<Net::TCPServer> svr = o.GetNN<Net::TCPServer>();
	UTF8Char buff[1024];
	UnsafeArray<UTF8Char> str;
	UIntOS sthreadCnt = 0;
	UIntOS i;
	Bool found;
	SubthreadStatus *sthreads = 0;
	Sync::Event *threadEvt = 0;

	Sync::ThreadUtil::SetPriority(Sync::ThreadUtil::TP_HIGHEST);
	Sync::ThreadUtil::SetName(CSTR("TCPSvrv4"));
	svr->threadRunning |= 1;
	str = Text::StrConcatC(buff, UTF8STRC("Start listening to v4 port "));
	str = Text::StrInt32(str, svr->port);
	svr->AddLogMsgC(CSTRP(buff, str), IO::LogHandler::LogLevel::Action);

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
	NN<Socket> soc;
	if (svr->svrSocv4.SetTo(soc))
	{
		while (!svr->toStop)
		{
			svr->AcceptSocket(soc);
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
	if (svr->svrSocv4.SetTo(soc))
	{
		svr->socf->DestroySocket(soc);
		svr->svrSocv4 = nullptr;
	}
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
	svr->AddLogMsgC(CSTRP(buff, str), IO::LogHandler::LogLevel::Action);
	svr->threadRunning &= ~1;
	return 0;
}

UInt32 __stdcall Net::TCPServer::Svrv6Subthread(AnyType o)
{
	NN<SubthreadStatus> status = o.GetNN<SubthreadStatus>();
	NN<Socket> soc;
	Sync::ThreadUtil::SetName(CSTR("TCPSvrv6Sub"));
	status->threadRunning = true;
	status->threadEvt->Set();
	if (status->me->svrSocv6.SetTo(soc))
	{
		while (!status->toStop)
		{
			status->me->AcceptSocket(soc);
		}
	}
	status->threadRunning = false;
	status->threadEvt->Set();
	return 0;
}

UInt32 __stdcall Net::TCPServer::Svrv6Thread(AnyType o)
{
	NN<Net::TCPServer> svr = o.GetNN<Net::TCPServer>();
	UTF8Char buff[1024];
	UnsafeArray<UTF8Char> str;
	UIntOS sthreadCnt = 0;
	UIntOS i;
	Bool found;
	SubthreadStatus *sthreads = 0;
	Sync::Event *threadEvt = 0;
	NN<Socket> soc;

	Sync::ThreadUtil::SetPriority(Sync::ThreadUtil::TP_HIGHEST);
	Sync::ThreadUtil::SetName(CSTR("TCPSvrv6"));
	svr->threadRunning |= 4;
	str = Text::StrConcatC(buff, UTF8STRC("Start listening to v6 port "));
	str = Text::StrInt32(str, svr->port);
	svr->AddLogMsgC(CSTRP(buff, str), IO::LogHandler::LogLevel::Action);

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
	if (svr->svrSocv6.SetTo(soc))
	{
		while (!svr->toStop)
		{
			svr->AcceptSocket(soc);
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
	if (svr->svrSocv6.SetTo(soc))
	{
		svr->socf->DestroySocket(soc);
		svr->svrSocv6 = nullptr;
	}
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
	svr->AddLogMsgC(CSTRP(buff, str), IO::LogHandler::LogLevel::Action);
	svr->threadRunning &= ~4;
	return 0;
}

UInt32 __stdcall Net::TCPServer::WorkerThread(AnyType o)
{
	NN<Net::TCPServer> svr = o.GetNN<Net::TCPServer>();
	UTF8Char buff[256];
	UnsafeArray<UTF8Char> str;

	Sync::ThreadUtil::SetName(CSTR("TCPSvrWork"));
	svr->threadRunning |= 2;

	while (!svr->toStop)
	{
		while (svr->socs.HasItems())
		{
			NN<Socket> s;
			if (svr->socs.Get().GetOpt<Socket>().SetTo(s))
			{
				str = Text::StrConcatC(buff, UTF8STRC("Client connected: "));
				str = svr->socf->GetRemoteName(str, s).Or(str);
				svr->AddLogMsgC(CSTRP(buff, str), IO::LogHandler::LogLevel::Action);
				svr->hdlr.func(s, svr->hdlr.userObj);
			}
		}
		svr->socsEvt.Wait(100);
	}
	svr->threadRunning &= ~2;
	return 0;
}

void Net::TCPServer::AcceptSocket(NN<Socket> svrSoc)
{
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	NN<Socket> s;
	if (!this->socf->SocketAccept(svrSoc).SetTo(s))
	{
		Int32 errCode = this->socf->SocketGetLastError();
		sptr = Text::StrConcatC(sbuff, UTF8STRC("Client connect error: "));
		sptr = Text::StrInt32(sptr, errCode);
		this->AddLogMsgC(CSTRP(sbuff, sptr), IO::LogHandler::LogLevel::ErrorDetail);
		if (errCode == 24) // too many opened files
		{
			Sync::SimpleThread::Sleep(2000);
		}
	}
	else
	{
		if (this->socs.GetLastNoRemove() == s.Ptr())
		{
		}
		else
		{
			this->socs.Put(s.Ptr());
			this->socsEvt.Set();
		}
/*		str = Text::StrConcatC(buff, UTF8STRC("Client connected: ");
		str = this->socf->GetRemoteName(str, (UInt32*)s);
		this->AddLogMsgC(buff, (UIntOS)(str - buff), IO::LogHandler::LogLevel::Action);
		this->hdlr((UInt32*)s, this->userObj);*/
	}	
}
Net::TCPServer::TCPServer(NN<SocketFactory> socf, Optional<Net::SocketUtil::AddressInfo> bindAddr, UInt16 port, NN<IO::LogTool> log, TCPServerConn hdlr, AnyType userObj, Text::CString logPrefix, Bool autoStart)
{
	Net::SocketUtil::AddressInfo addrTmp;
	NN<Net::SocketUtil::AddressInfo> addr;
	UTF8Char buff[1024];
	UnsafeArray<UTF8Char> str;
	this->socf = socf;
	this->toStop = false;
	this->errorv4 = false;
	this->errorv6 = false;
	this->port = port;
	this->log = log;
	this->svrSocv4 = nullptr;
	this->svrSocv6 = nullptr;
	this->logPrefix = Text::String::NewOrNull(logPrefix);
	this->hdlr = {hdlr, userObj};
	this->threadRunning = 0;

	UInt32 bindIP = 0;
	if (bindAddr.SetTo(addr) && addr->addrType == Net::AddrType::IPv4)
	{
		bindIP = *(UInt32*)addr->addr;
	}
	NN<Socket> soc;
	this->svrSocv4 = this->socf->CreateTCPSocketv4();
	if (!this->svrSocv4.SetTo(soc))
	{
		this->AddLogMsgC(CSTR("Error in create TCPv4 socket"), IO::LogHandler::LogLevel::Error);
	}
	else if (!this->socf->SocketBindv4(soc, bindIP, this->port))
	{
		str = Text::StrConcatC(buff, UTF8STRC("Cannot bind to the v4 port: "));
		str = Text::StrInt32(str, this->port);
		this->AddLogMsgC(CSTRP(buff, str), IO::LogHandler::LogLevel::Error);
		this->socf->DestroySocket(soc);
		this->svrSocv4 = nullptr;
		this->errorv4 = true;
	}
	else if (!this->socf->SocketListen(soc))
	{
		str = Text::StrConcatC(buff, UTF8STRC("Cannot start listening the v4 port: "));
		str = Text::StrInt32(str, this->port);
		this->AddLogMsgC(CSTRP(buff, str), IO::LogHandler::LogLevel::Error);
		this->socf->DestroySocket(soc);
		this->svrSocv4 = nullptr;
		this->errorv4 = true;
	}
	else if (this->port == 0)
	{
		this->socf->GetLocalAddr(soc, addrTmp, this->port);
	}

	Net::SocketUtil::SetAddrInfoAnyV6(addrTmp);
	Bool skipV6 = false;
	if (bindAddr.SetTo(addr))
	{
		if (addr->addrType == Net::AddrType::IPv4)
		{
			skipV6 = true;
		}
		else if (addr->addrType == Net::AddrType::IPv6)
		{
			addr.SetVal(addrTmp);
		}
	}

	if (skipV6)
	{
		this->svrSocv6 = nullptr;
		this->errorv6 = true;
	}
	else
	{
		this->svrSocv6 = this->socf->CreateTCPSocketv6();
		if (!this->svrSocv6.SetTo(soc))
		{
			this->AddLogMsgC(CSTR("Error in creating TCPv6 socket"), IO::LogHandler::LogLevel::Error);
			this->errorv6 = true;
		}
		else if (!this->socf->SocketBind(soc, &addrTmp, this->port))
		{
			str = Text::StrConcatC(buff, UTF8STRC("Cannot bind to the v6 port: "));
			str = Text::StrInt32(str, this->port);
			this->AddLogMsgC(CSTRP(buff, str), IO::LogHandler::LogLevel::Error);
			this->socf->DestroySocket(soc);
			this->svrSocv6 = nullptr;
			this->errorv6 = true;
		}
		else if (!this->socf->SocketListen(soc))
		{
			str = Text::StrConcatC(buff, UTF8STRC("Cannot start listening the v6 port: "));
			str = Text::StrInt32(str, this->port);
			this->AddLogMsgC(CSTRP(buff, str), IO::LogHandler::LogLevel::Error);
			this->socf->DestroySocket(soc);
			this->svrSocv6 = nullptr;
			this->errorv6 = true;
		}
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
		NN<Socket> soc;
		toStop = true;
		if (this->svrSocv4.SetTo(soc))
		{
			socf->DestroySocket(soc);
		}
		if (this->svrSocv6.SetTo(soc))
		{
			socf->DestroySocket(soc);
		}
		this->socsEvt.Set();
		this->errorv4 = true;
		this->errorv6 = true;
		this->svrSocv4 = nullptr;
		this->svrSocv6 = nullptr;
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

UInt16 Net::TCPServer::GetListenPort() const
{
	return this->port;
}
