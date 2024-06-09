#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Net/UDPServer.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"

UInt32 __stdcall Net::UDPServer::DataV4Thread(AnyType obj)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	NN<Text::String> s;
	NN<Net::UDPServer::ThreadStat> stat = obj.GetNN<Net::UDPServer::ThreadStat>();
	NN<Socket> soc;
	if (stat->me->socV4.SetTo(soc))
	{
		Sync::Event evt;
		stat->evt = &evt;
		stat->threadRunning = true;
		stat->me->ctrlEvt.Set();

		UInt8 *buff = MemAlloc(UInt8, 2048);
		while (!stat->toStop)
		{
			UOSInt recvSize;
			Net::SocketUtil::AddressInfo recvAddr;
			UInt16 recvPort;

			recvSize = stat->me->sockf->UDPReceive(soc, buff, 2048, recvAddr, recvPort, 0);
			Data::Timestamp logTime = Data::Timestamp::UtcNow();
			if (recvSize > 0)
			{
				Sync::Interlocked::IncrementI32(stat->me->recvCnt);
				if (stat->me->msgLog->HasHandler())
				{
					if (stat->me->msgPrefix.SetTo(s))
						sptr = s->ConcatTo(sbuff);
					else
						sptr = sbuff;
					sptr = Text::StrConcatC(sptr, UTF8STRC("Received "));
					sptr = Text::StrUOSInt(sptr, recvSize);
					sptr = Text::StrConcatC(sptr, UTF8STRC(" bytes from "));
					sptr = Net::SocketUtil::GetAddrName(sptr, recvAddr, recvPort).Or(sptr);
					stat->me->msgLog->LogMessage(CSTRP(sbuff, sptr), IO::LogHandler::LogLevel::Raw);
				}

				if (stat->me->logPrefix.SetTo(s))
				{
					Sync::MutexUsage mutUsage(stat->me->logFileMut);
					if ((!logTime.SameDate(stat->me->logDateR)) || (stat->me->logFileR == 0))
					{
						if (stat->me->logFileR)
						{
							DEL_CLASS(stat->me->logFileR);
							stat->me->logFileR = 0;
						}
						sptr = s->ConcatTo(sbuff);
						sptr = logTime.ToString(sptr, "yyyyMMdd");
						sptr = Text::StrConcatC(sptr, UTF8STRC("r.udp"));
						NEW_CLASS(stat->me->logFileR, IO::FileStream(CSTRP(sbuff, sptr), IO::FileMode::Append, IO::FileShare::DenyWrite, IO::FileStream::BufferType::Normal));
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
				stat->me->hdlr.func(recvAddr, recvPort, Data::ByteArrayR(buff, recvSize), stat->me->hdlr.userObj);
			}
		}
		MemFree(buff);
	}
	stat->threadRunning = false;
	stat->me->ctrlEvt.Set();
	return 0;
}

UInt32 __stdcall Net::UDPServer::DataV6Thread(AnyType obj)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	NN<Text::String> s;
	NN<Net::UDPServer::ThreadStat> stat = obj.GetNN<Net::UDPServer::ThreadStat>();
	NN<Socket> soc;
	if (stat->me->socV6.SetTo(soc))
	{
		Sync::Event evt;
		stat->evt = &evt;
		stat->threadRunning = true;
		stat->me->ctrlEvt.Set();

		UInt8 *buff = MemAlloc(UInt8, 2048);
		while (!stat->toStop)
		{
			UOSInt recvSize;
			Net::SocketUtil::AddressInfo recvAddr;
			UInt16 recvPort;

			recvSize = stat->me->sockf->UDPReceive(soc, buff, 2048, recvAddr, recvPort, 0);
			Data::Timestamp logTime = Data::Timestamp::UtcNow();
			if (recvSize > 0)
			{
				Sync::Interlocked::IncrementI32(stat->me->recvCnt);
				if (stat->me->msgLog->HasHandler())
				{
					if (stat->me->msgPrefix.SetTo(s))
						sptr = s->ConcatTo(sbuff);
					else
						sptr = sbuff;
					sptr = Text::StrConcatC(sptr, UTF8STRC("Received "));
					sptr = Text::StrUOSInt(sptr, recvSize);
					sptr = Text::StrConcatC(sptr, UTF8STRC(" bytes from "));
					sptr = Net::SocketUtil::GetAddrName(sptr, recvAddr, recvPort).Or(sptr);
					stat->me->msgLog->LogMessage(CSTRP(sbuff, sptr), IO::LogHandler::LogLevel::Raw);
				}

				if (stat->me->logPrefix.SetTo(s))
				{
					Sync::MutexUsage mutUsage(stat->me->logFileMut);
					if ((!logTime.SameDate(stat->me->logDateR)) || (stat->me->logFileR == 0))
					{
						if (stat->me->logFileR)
						{
							DEL_CLASS(stat->me->logFileR);
							stat->me->logFileR = 0;
						}
						sptr = s->ConcatTo(sbuff);
						sptr = logTime.ToString(sptr, "yyyyMMdd");
						sptr = Text::StrConcatC(sptr, UTF8STRC("r.udp"));
						NEW_CLASS(stat->me->logFileR, IO::FileStream(CSTRP(sbuff, sptr), IO::FileMode::Append, IO::FileShare::DenyWrite, IO::FileStream::BufferType::Normal));
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
				stat->me->hdlr.func(recvAddr, recvPort, Data::ByteArrayR(buff, recvSize), stat->me->hdlr.userObj);
			}
		}
		MemFree(buff);
	}
	stat->threadRunning = false;
	stat->me->ctrlEvt.Set();
	return 0;
}

Net::UDPServer::UDPServer(NN<Net::SocketFactory> sockf, Net::SocketUtil::AddressInfo *bindAddr, UInt16 port, Text::CString logPrefix, UDPPacketHdlr hdlr, AnyType userData, NN<IO::LogTool> msgLog, Text::CString msgPrefix, UOSInt threadCnt, Bool reuseAddr)
{
	this->threadCnt = threadCnt;
	this->v4threadStats = 0;
	this->v6threadStats = 0;
	this->recvCnt = 0;
	UOSInt i;

	this->sockf = sockf;
	this->logPrefix = Text::String::NewOrNull(logPrefix);
	this->hdlr = {hdlr, userData};
	this->logFileR = 0;
	this->logFileS = 0;
	this->msgLog = msgLog;
	this->msgPrefix = Text::String::NewOrNull(msgPrefix);
	this->port = port;
	Bool succ = false;
	NN<Socket> soc;
	if (bindAddr == 0)
	{
		this->addrType = Net::AddrType::Unknown;
		this->socV4 = this->sockf->CreateUDPSocketv4();
		this->socV6 = this->sockf->CreateUDPSocketv6();
		if (reuseAddr)
		{
			if (this->socV4.SetTo(soc)) this->sockf->SetReuseAddr(soc, true);
			if (this->socV6.SetTo(soc)) this->sockf->SetReuseAddr(soc, true);
		}
		if (this->socV4.SetTo(soc) && this->sockf->SocketBindv4(soc, 0, port))
		{
			if (this->socV6.SetTo(soc))
			{
				Net::SocketUtil::AddressInfo addrAny;
				Net::SocketUtil::SetAddrInfoAnyV6(addrAny);
				if (!this->sockf->SocketBind(soc, &addrAny, port))
				{
					this->sockf->DestroySocket(soc);
					this->socV6 = 0;
				}
			}
			succ = true;
		}
		else
		{
			if (this->socV4.SetTo(soc))
			{
				this->sockf->DestroySocket(soc);
				this->socV4 = 0;
			}
			if (this->socV6.SetTo(soc))
			{
				this->sockf->DestroySocket(soc);
				this->socV6 = 0;
			}
		}
	}
	else
	{
		this->addrType = bindAddr->addrType;
		this->socV4 = 0;
		this->socV6 = 0;
		if (this->addrType == Net::AddrType::IPv4)
		{
			this->socV4 = this->sockf->CreateUDPSocketv4();
			if (reuseAddr && this->socV4.SetTo(soc))
			{
				this->sockf->SetReuseAddr(soc, true);
			}
			if (this->socV4.SetTo(soc) && this->sockf->SocketBindv4(soc, ReadNUInt32(bindAddr->addr), port))
			{
				succ = true;
			}
			else if (this->socV4.SetTo(soc))
			{
				this->sockf->DestroySocket(soc);
				this->socV4 = 0;
			}
		}
		else if (this->addrType == Net::AddrType::IPv6)
		{
			this->socV6 = this->sockf->CreateUDPSocketv6();
			if (reuseAddr && this->socV6.SetTo(soc))
			{
				this->sockf->SetReuseAddr(soc, true);
			}
			if (this->socV6.SetTo(soc) && this->sockf->SocketBind(soc, bindAddr, port))
			{
				succ = true;
			}
			else if (this->socV6.SetTo(soc))
			{
				this->sockf->DestroySocket(soc);
				this->socV6 = 0;
			}

		}
	}
	if (succ)
	{
		if (port == 0 && this->socV4.SetTo(soc))
		{
			Net::SocketUtil::AddressInfo addr;
			this->sockf->GetLocalAddr(soc, addr, this->port);
		}
		if (!this->logPrefix.IsNull())
		{
			this->logDateR = Data::Timestamp::UtcNow().AddDay(-1);
			this->logDateS = this->logDateR;
		}
		else
		{
			this->logDateR = Data::Timestamp(0);
			this->logDateS = Data::Timestamp(0);
		}

		this->v4threadStats = MemAlloc(ThreadStat, this->threadCnt);
		if (this->socV6.NotNull())
		{
			this->v6threadStats = MemAlloc(ThreadStat, this->threadCnt);
		}
		i = this->threadCnt;
		while (i-- > 0)
		{
			this->v4threadStats[i].toStop = false;
			this->v4threadStats[i].threadRunning = false;
			this->v4threadStats[i].me = this;
			Sync::ThreadUtil::Create(DataV4Thread, &this->v4threadStats[i]);

			if (this->socV6.NotNull())
			{
				this->v6threadStats[i].toStop = false;
				this->v6threadStats[i].threadRunning = false;
				this->v6threadStats[i].me = this;
				Sync::ThreadUtil::Create(DataV6Thread, &this->v6threadStats[i]);
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
				if (this->socV6.NotNull())
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
			this->ctrlEvt.Wait(10);
		}
	}
	else
	{
		this->logDateR = Data::Timestamp(0);
		this->logDateS = Data::Timestamp(0);
	}
}

Net::UDPServer::~UDPServer()
{
	NN<Socket> soc;
	UOSInt i;
	if (this->v4threadStats)
	{
		i = this->threadCnt;
		while (i-- > 0)
		{
			this->v4threadStats[i].toStop = true;
			this->v4threadStats[i].evt->Set();
		}
	}
	if (this->v6threadStats)
	{
		i = this->threadCnt;
		while (i-- > 0)
		{
			this->v6threadStats[i].toStop = true;
			this->v6threadStats[i].evt->Set();
		}
	}
	if (this->socV4.SetTo(soc))
	{
		this->sockf->DestroySocket(soc);
	}
	if (this->socV6.SetTo(soc))
	{
		this->sockf->DestroySocket(soc);
	}
	if (this->v4threadStats)
	{
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
				if (this->socV6.NotNull())
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
			this->ctrlEvt.Wait(10);
		}

		MemFree(this->v4threadStats);
		if (this->socV6.NotNull())
		{
			MemFree(this->v6threadStats);
		}
	}
	this->socV4 = 0;
	this->socV6 = 0;

	SDEL_CLASS(this->logFileS);
	SDEL_CLASS(this->logFileR);
	OPTSTR_DEL(this->logPrefix);
	OPTSTR_DEL(this->msgPrefix);
}

UInt16 Net::UDPServer::GetPort()
{
	return this->port;
}

Bool Net::UDPServer::IsError()
{
	return this->socV4.IsNull();
}

Bool Net::UDPServer::SupportV6()
{
	return this->socV6.NotNull();
}

Bool Net::UDPServer::SendTo(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, UnsafeArray<const UInt8> buff, UOSInt dataSize)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	Bool succ;
	NN<Text::String> s;
	if (this->logPrefix.SetTo(s))
	{
		Data::Timestamp logTime = Data::Timestamp::UtcNow();

		Sync::MutexUsage mutUsage(this->logFileMut);
		if ((!logTime.SameDate(this->logDateS)) || (logFileS == 0))
		{
			if (logFileS)
			{
				DEL_CLASS(logFileS);
			}
			logFileS = 0;

			sptr = s->ConcatTo(sbuff);
			sptr = logTime.ToString(sptr, "yyyyMMdd");
			sptr = Text::StrConcatC(sptr, UTF8STRC("s.udp"));
			NEW_CLASS(this->logFileS, IO::FileStream(CSTRP(sbuff, sptr), IO::FileMode::Append, IO::FileShare::DenyWrite, IO::FileStream::BufferType::Normal));
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

	if (this->msgLog->HasHandler())
	{
		if (msgPrefix.SetTo(s))
		{
			sptr = s->ConcatTo(sbuff);
		}
		else
		{
			sptr = sbuff;
		}
		sptr = Text::StrConcatC(sptr, UTF8STRC("Sending UDP "));
		sptr = Text::StrUOSInt(sptr, dataSize);
		sptr = Text::StrConcatC(sptr, UTF8STRC(" bytes to "));
		sptr = Net::SocketUtil::GetAddrName(sptr, addr, port).Or(sptr);
		this->msgLog->LogMessage(CSTRP(sbuff, sptr), IO::LogHandler::LogLevel::Raw);
	}

	succ = false;
	NN<Socket> soc;
	if (addr->addrType == Net::AddrType::IPv4)
	{
//		printf("IPv4 port = %d\r\n", port);
		if (!this->socV4.SetTo(soc) || this->sockf->SendTo(soc, buff, dataSize, addr, port) != dataSize)
		{
//			printf("Send error: %d\r\n", errno);
			if (this->msgLog->HasHandler())
			{
				if (msgPrefix.SetTo(s))
					sptr = s->ConcatTo(sbuff);
				else
					sptr = sbuff;
				sptr = Text::StrConcatC(sptr, UTF8STRC("Send UDP data failed"));
				this->msgLog->LogMessage(CSTRP(sbuff, sptr), IO::LogHandler::LogLevel::Error);
			}
		}
		else
		{
			succ = true;
		}
	}
	else if (addr->addrType == Net::AddrType::IPv6)
	{
		if (!this->socV6.SetTo(soc) || this->sockf->SendTo(soc, buff, dataSize, addr, port) != dataSize)
		{
			if (this->msgLog->HasHandler())
			{
				if (msgPrefix.SetTo(s))
					sptr = s->ConcatTo(sbuff);
				else
					sptr = sbuff;
				sptr = Text::StrConcatC(sptr, UTF8STRC("Send UDP data failed"));
				this->msgLog->LogMessage(CSTRP(sbuff, sptr), IO::LogHandler::LogLevel::Error);
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
	NN<Socket> soc;
	if (this->socV4.SetTo(soc))
	{
		this->sockf->AddIPMembership(soc, ip);
	}
}

void Net::UDPServer::SetBuffSize(Int32 buffSize)
{
	NN<Socket> soc;
	if (this->socV4.SetTo(soc))
	{
		this->sockf->SetRecvBuffSize(soc, buffSize);
	}
	if (this->socV6.SetTo(soc))
	{
		this->sockf->SetRecvBuffSize(soc, buffSize);
	}
}

void Net::UDPServer::SetBroadcast(Bool val)
{
	NN<Socket> soc;
	if (this->socV4.SetTo(soc))
	{
		this->sockf->SetBroadcast(soc, val);
	}
	if (this->socV6.SetTo(soc))
	{
		this->sockf->SetBroadcast(soc, val);
	}
}
