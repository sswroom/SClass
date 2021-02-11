#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Net/UDPServer.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"

UInt32 __stdcall Net::UDPServer::DataV4Thread(void *obj)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	Net::UDPServer::ThreadStat *stat = (Net::UDPServer::ThreadStat*)obj;
	stat->threadRunning = true;
	stat->me->ctrlEvt->Set();

	UInt8 *buff = MemAlloc(UInt8, 2048);
	while (!stat->toStop)
	{
		OSInt recvSize;
		Net::SocketUtil::AddressInfo recvAddr;
		UInt16 recvPort;

		recvSize = stat->me->sockf->UDPReceive(stat->me->socV4, buff, 2048, &recvAddr, &recvPort, 0);
		Data::DateTime logTime;
		logTime.SetCurrTimeUTC();
		if (recvSize > 0)
		{
			Sync::Interlocked::Increment(&stat->me->recvCnt);
			if (stat->me->msgLog)
			{
				if (stat->me->msgPrefix)
					sptr = Text::StrConcat(sbuff, stat->me->msgPrefix);
				else
					sptr = sbuff;
				sptr = Text::StrConcat(sptr, (const UTF8Char*)"Received ");
				sptr = Text::StrInt32(sptr, (Int32)recvSize);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)" bytes from ");
				sptr = Net::SocketUtil::GetAddrName(sptr, &recvAddr, recvPort);
				stat->me->msgLog->LogMessage(sbuff, IO::ILogHandler::LOG_LEVEL_RAW);
			}

			if (stat->me->logPrefix)
			{
				Sync::MutexUsage mutUsage(stat->me->logFileMut);
				if ((logTime.GetDay() != stat->me->logDateR->GetDay()) || (stat->me->logFileR == 0))
				{
					if (stat->me->logFileR)
					{
						DEL_CLASS(stat->me->logFileR);
						stat->me->logFileR = 0;
					}
					sptr = Text::StrConcat(sbuff, stat->me->logPrefix);
					sptr = logTime.ToString(sptr, "yyyyMMdd");
					sptr = Text::StrConcat(sptr, (const UTF8Char*)"r.udp");
					NEW_CLASS(stat->me->logFileR, IO::FileStream(sbuff, IO::FileStream::FILE_MODE_APPEND, IO::FileStream::FILE_SHARE_DENY_WRITE, IO::FileStream::BT_NORMAL));
				}

				if (stat->me->logFileR)
				{
					Int32 v = (Int32)(logTime.ToUnixTimestamp() & 0xffffffffLL);
					UInt8 hbuff[8];
					hbuff[0] = 0xaa;
					hbuff[1] = 0xbb;
					WriteInt16(&hbuff[2], (Int16)recvSize);
					WriteInt32(&hbuff[4], v);
					stat->me->logFileR->Write(hbuff, 8);
					stat->me->logFileR->Write(buff, recvSize);
				}
				mutUsage.EndUse();
			}
			stat->me->hdlr(&recvAddr, recvPort, buff, recvSize, stat->me->userData);
		}
	}
	MemFree(buff);
	stat->threadRunning = false;
	stat->me->ctrlEvt->Set();
	return 0;
}

UInt32 __stdcall Net::UDPServer::DataV6Thread(void *obj)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	Net::UDPServer::ThreadStat *stat = (Net::UDPServer::ThreadStat*)obj;
	stat->threadRunning = true;
	stat->me->ctrlEvt->Set();

	UInt8 *buff = MemAlloc(UInt8, 2048);
	while (!stat->toStop)
	{
		OSInt recvSize;
		Net::SocketUtil::AddressInfo recvAddr;
		UInt16 recvPort;

		recvSize = stat->me->sockf->UDPReceive(stat->me->socV6, buff, 2048, &recvAddr, &recvPort, 0);
		Data::DateTime logTime;
		logTime.SetCurrTimeUTC();
		if (recvSize > 0)
		{
			Sync::Interlocked::Increment(&stat->me->recvCnt);
			if (stat->me->msgLog)
			{
				if (stat->me->msgPrefix)
					sptr = Text::StrConcat(sbuff, stat->me->msgPrefix);
				else
					sptr = sbuff;
				sptr = Text::StrConcat(sptr, (const UTF8Char*)"Received ");
				sptr = Text::StrInt32(sptr, (Int32)recvSize);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)" bytes from ");
				sptr = Net::SocketUtil::GetAddrName(sptr, &recvAddr, recvPort);
				stat->me->msgLog->LogMessage(sbuff, IO::ILogHandler::LOG_LEVEL_RAW);
			}

			if (stat->me->logPrefix)
			{
				Sync::MutexUsage mutUsage(stat->me->logFileMut);
				if ((logTime.GetDay() != stat->me->logDateR->GetDay()) || (stat->me->logFileR == 0))
				{
					if (stat->me->logFileR)
					{
						DEL_CLASS(stat->me->logFileR);
						stat->me->logFileR = 0;
					}
					sptr = Text::StrConcat(sbuff, stat->me->logPrefix);
					sptr = logTime.ToString(sptr, "yyyyMMdd");
					sptr = Text::StrConcat(sptr, (const UTF8Char*)"r.udp");
					NEW_CLASS(stat->me->logFileR, IO::FileStream(sbuff, IO::FileStream::FILE_MODE_APPEND, IO::FileStream::FILE_SHARE_DENY_WRITE, IO::FileStream::BT_NORMAL));
				}

				if (stat->me->logFileR)
				{
					Int32 v = (Int32)(logTime.ToUnixTimestamp() & 0xffffffffLL);
					UInt8 hbuff[8];
					hbuff[0] = 0xaa;
					hbuff[1] = 0xbb;
					WriteInt16(&hbuff[2], (Int16)recvSize);
					WriteInt32(&hbuff[4], v);
					stat->me->logFileR->Write(hbuff, 8);
					stat->me->logFileR->Write(buff, recvSize);
				}
				mutUsage.EndUse();
			}
			stat->me->hdlr(&recvAddr, recvPort, buff, recvSize, stat->me->userData);
		}
	}
	MemFree(buff);
	stat->threadRunning = false;
	stat->me->ctrlEvt->Set();
	return 0;
}

Net::UDPServer::UDPServer(Net::SocketFactory *sockf, Net::SocketUtil::AddressInfo *bindAddr, UInt16 port, const UTF8Char *logPrefix, UDPPacketHdlr hdlr, void *userData, IO::LogTool *msgLog, const UTF8Char *msgPrefix, OSInt threadCnt, Bool reuseAddr)
{
	this->threadCnt = threadCnt;
	this->v4threadStats = 0;
	this->v6threadStats = 0;
	this->recvCnt = 0;
	OSInt i;

	this->sockf = sockf;
	if (logPrefix)
	{
		this->logPrefix = Text::StrCopyNew(logPrefix);
	}
	else
	{
		this->logPrefix = 0;
	}
	this->hdlr = hdlr;
	this->userData = userData;
	this->logFileR = 0;
	this->logFileS = 0;
	this->ctrlEvt = 0;
	NEW_CLASS(this->logFileMut, Sync::Mutex());
	this->msgLog = msgLog;
	if (msgPrefix)
	{
		this->msgPrefix = Text::StrCopyNew(msgPrefix);
	}
	else
	{
		this->msgPrefix = 0;
	}
	this->port = port;
	Bool succ = false;
	if (bindAddr == 0)
	{
		this->addrType = Net::SocketUtil::AT_UNKNOWN;
		this->socV4 = this->sockf->CreateUDPSocketv4();
		this->socV6 = this->sockf->CreateUDPSocketv6();
		if (reuseAddr)
		{
			this->sockf->SetReuseAddr(this->socV4, true);
			this->sockf->SetReuseAddr(this->socV6, true);
		}
		if (this->sockf->SocketBindv4(this->socV4, 0, port))
		{
			Net::SocketUtil::AddressInfo addrAny;
			Net::SocketUtil::SetAddrAnyV6(&addrAny);
			if (!this->sockf->SocketBind(this->socV6, &addrAny, port))
			{
				this->sockf->DestroySocket(this->socV6);
				this->socV6 = 0;
			}
			succ = true;
		}
		else
		{
			this->sockf->DestroySocket(this->socV4);
			this->socV4 = 0;
		}
	}
	else
	{
		this->addrType = bindAddr->addrType;
		this->socV4 = 0;
		this->socV6 = 0;
		if (this->addrType == Net::SocketUtil::AT_IPV4)
		{
			this->socV4 = this->sockf->CreateUDPSocketv4();
			if (reuseAddr)
			{
				this->sockf->SetReuseAddr(this->socV4, true);
			}
			if (this->sockf->SocketBindv4(this->socV4, ReadNInt32(bindAddr->addr), port))
			{
				succ = true;
			}
			else
			{
				this->sockf->DestroySocket(this->socV4);
				this->socV4 = 0;
			}
		}
		else if (this->addrType == Net::SocketUtil::AT_IPV6)
		{
			this->socV6 = this->sockf->CreateUDPSocketv6();
			if (reuseAddr)
			{
				this->sockf->SetReuseAddr(this->socV6, true);
			}
			if (this->sockf->SocketBind(this->socV6, bindAddr, port))
			{
				succ = true;
			}
			else
			{
				this->sockf->DestroySocket(this->socV6);
				this->socV6 = 0;
			}

		}
	}
	if (succ)
	{
		if (port == 0)
		{
			this->sockf->GetLocalAddr(this->socV4, 0, &this->port);
		}
		if (this->logPrefix)
		{
			NEW_CLASS(this->logDateR, Data::DateTime());
			NEW_CLASS(this->logDateS, Data::DateTime());
			this->logDateR->SetCurrTimeUTC();
			this->logDateS->SetCurrTimeUTC();
			this->logDateR->AddDay(-1);
			this->logDateS->AddDay(-1);
		}
		else
		{
			this->logDateR = 0;
			this->logDateS = 0;
		}
		NEW_CLASS(this->ctrlEvt, Sync::Event(true, (const UTF8Char*)"Net.UDPServer.ctrlEvt"));

		this->v4threadStats = MemAlloc(ThreadStat, this->threadCnt);
		if (this->socV6)
		{
			this->v6threadStats = MemAlloc(ThreadStat, this->threadCnt);
		}
		i = this->threadCnt;
		while (i-- > 0)
		{
			this->v4threadStats[i].toStop = false;
			this->v4threadStats[i].threadRunning = false;
			NEW_CLASS(this->v4threadStats[i].evt, Sync::Event(true, (const UTF8Char*)"Net.UDPServer.threadEvt"));
			this->v4threadStats[i].me = this;
			Sync::Thread::Create(DataV4Thread, &this->v4threadStats[i]);

			if (this->socV6)
			{
				this->v6threadStats[i].toStop = false;
				this->v6threadStats[i].threadRunning = false;
				NEW_CLASS(this->v6threadStats[i].evt, Sync::Event(true, (const UTF8Char*)"Net.UDPServer.threadEvt"));
				this->v6threadStats[i].me = this;
				Sync::Thread::Create(DataV6Thread, &this->v6threadStats[i]);
			}
		}
		Bool running;
		while (true)
		{
			running = true;
			i = this->threadCnt;
			while (i-- > 0)
			{
				if (!this->v4threadStats[i].threadRunning)
				{
					running = false;
					break;
				}
				if (this->socV6)
				{
					if (!this->v6threadStats[i].threadRunning)
					{
						running = false;
						break;
					}
				}
			}
			if (running)
				break;
			this->ctrlEvt->Wait(10);
		}
	}
	else
	{
		this->logDateR = 0;
		this->logDateS = 0;
	}
}

Net::UDPServer::~UDPServer()
{
	OSInt i;
	if (this->v4threadStats)
	{
		i = this->threadCnt;
		while (i-- > 0)
		{
			this->v4threadStats[i].toStop = true;
		}
	}
	if (this->v6threadStats)
	{
		i = this->threadCnt;
		while (i-- > 0)
		{
			this->v6threadStats[i].toStop = true;
		}
	}
	if (this->socV4)
	{
		this->sockf->DestroySocket(this->socV4);
	}
	if (this->socV6)
	{
		this->sockf->DestroySocket(this->socV6);
	}
	if (this->v4threadStats)
	{
		i = this->threadCnt;
		while (i-- > 0)
		{
			this->v4threadStats[i].evt->Set();
			if (this->socV6)
			{
				this->v6threadStats[i].evt->Set();
			}
		}

		Bool threadRunning = true;
		while (threadRunning)
		{
			threadRunning = false;
			i = this->threadCnt;
			while (i-- > 0)
			{
				if (this->v4threadStats[i].threadRunning)
				{
					threadRunning = true;
					break;
				}
				if (this->socV6)
				{
					if (this->v6threadStats[i].threadRunning)
					{
						threadRunning = true;
						break;
					}
				}
			}
			if (!threadRunning)
				break;
			this->ctrlEvt->Wait(10);
		}

		i = this->threadCnt;
		while (i-- > 0)
		{
			DEL_CLASS(this->v4threadStats[i].evt);
			if (this->socV6)
			{
				DEL_CLASS(this->v6threadStats[i].evt);
			}
		}
		MemFree(this->v4threadStats);
		if (this->socV6)
		{
			MemFree(this->v6threadStats);
		}
	}
	if (this->socV4)
	{
		this->sockf->DestroySocket(this->socV4);
		this->socV4 = 0;
	}
	if (this->socV6)
	{
		this->sockf->DestroySocket(this->socV6);
		this->socV6 = 0;
	}

	DEL_CLASS(this->logFileMut);
	SDEL_CLASS(this->logDateR);
	SDEL_CLASS(this->logDateS);
	SDEL_CLASS(this->logFileS);
	SDEL_CLASS(this->logFileR);
	SDEL_TEXT(this->logPrefix);
	SDEL_TEXT(this->msgPrefix);
	SDEL_CLASS(this->ctrlEvt);
}

UInt16 Net::UDPServer::GetPort()
{
	return this->port;
}

Bool Net::UDPServer::IsError()
{
	return this->socV4 == 0;
}

Bool Net::UDPServer::SupportV6()
{
	return this->socV6 != 0;
}

Bool Net::UDPServer::SendTo(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	Bool succ;
	if (this->logPrefix)
	{
		Data::DateTime logTime;
		logTime.SetCurrTimeUTC();

		Sync::MutexUsage mutUsage(this->logFileMut);
		if ((logTime.GetDay() != this->logDateS->GetDay()) || (logFileS == 0))
		{
			if (logFileS)
			{
				DEL_CLASS(logFileS);
			}
			logFileS = 0;

			sptr = Text::StrConcat(sbuff, this->logPrefix);
			sptr = logTime.ToString(sptr, "yyyyMMdd");
			sptr = Text::StrConcat(sptr, (const UTF8Char*)"s.udp");
			NEW_CLASS(this->logFileS, IO::FileStream(sbuff, IO::FileStream::FILE_MODE_APPEND, IO::FileStream::FILE_SHARE_DENY_WRITE, IO::FileStream::BT_NORMAL));
		}

		if (this->logFileS)
		{
			Int32 v = (Int32)(logTime.ToUnixTimestamp() & 0xffffffffLL);
			UInt8 hbuff[8];
			hbuff[0] = 0xaa;
			hbuff[1] = 0xbb;
			WriteInt16(&hbuff[2], (Int16)dataSize);
			WriteInt32(&hbuff[4], v);
			this->logFileS->Write(hbuff, 8);
			this->logFileS->Write(buff, dataSize);
		}
		mutUsage.EndUse();
	}

	if (this->msgLog)
	{
		if (msgPrefix)
		{
			sptr = Text::StrConcat(sbuff, msgPrefix);
		}
		else
		{
			sptr = sbuff;
		}
		sptr = Text::StrConcat(sptr, (const UTF8Char*)"Sending UDP ");
		sptr = Text::StrOSInt(sptr, dataSize);
		sptr = Text::StrConcat(sptr, (const UTF8Char*)" bytes to ");
		sptr = Net::SocketUtil::GetAddrName(sptr, addr, port);
		this->msgLog->LogMessage(sbuff, IO::ILogHandler::LOG_LEVEL_RAW);
	}

	succ = false;
	if (addr->addrType == Net::SocketUtil::AT_IPV4)
	{
//		printf("IPv4 port = %d\r\n", port);
		if (this->sockf->SendTo(this->socV4, buff, dataSize, addr, port) != dataSize)
		{
//			printf("Send error: %d\r\n", errno);
			if (this->msgLog)
			{
				sptr = Text::StrConcat(sbuff, msgPrefix);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)"Send UDP data failed");
				this->msgLog->LogMessage(sbuff, IO::ILogHandler::LOG_LEVEL_ERROR);
			}
		}
		else
		{
			succ = true;
		}
	}
	else if (addr->addrType == Net::SocketUtil::AT_IPV6)
	{
		if (this->sockf->SendTo(this->socV6, buff, dataSize, addr, port) != dataSize)
		{
			if (this->msgLog)
			{
				sptr = Text::StrConcat(sbuff, msgPrefix);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)"Send UDP data failed");
				this->msgLog->LogMessage(sbuff, IO::ILogHandler::LOG_LEVEL_ERROR);
			}
		}
		else
		{
			succ = true;
		}
	}
	return succ;
}

Int32 Net::UDPServer::GetRecvCnt()
{
	return this->recvCnt;
}

void Net::UDPServer::AddMulticastIP(UInt32 ip)
{
	this->sockf->AddIPMembership(this->socV4, ip);
}

void Net::UDPServer::SetBuffSize(Int32 buffSize)
{
	this->sockf->SetRecvBuffSize(this->socV4, buffSize);
	if (this->socV6)
	{
		this->sockf->SetRecvBuffSize(this->socV6, buffSize);
	}
}

void Net::UDPServer::SetBroadcast(Bool val)
{
	this->sockf->SetBroadcast(this->socV4, val);
	if (this->socV6)
	{
		this->sockf->SetBroadcast(this->socV6, val);
	}
}
