#include "Stdafx.h"

#include "MyMemory.h"
#include "IO/Stream.h"
#include "Net/SocketFactory.h"
#include "Net/TCPClient.h"
#include "Sync/Event.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"

#include <stdio.h>
//#define PRINTDEBUG
#ifdef PRINTDEBUG
#include "IO/Console.h"
#include "Text/StringBuilderUTF8.h"
#endif

Net::TCPClient::TCPClient(NN<Net::SocketFactory> sockf, Text::CStringNN name, UInt16 port, Data::Duration timeout) : IO::Stream(name)
{
	this->currCnt = 0;
	this->flags = 0;
	this->s = 0;
	this->sockf = sockf;
	this->readEvent = 0;
	this->writeEvent = 0;
	this->timeout = 0;
	this->cliId = 0;

	NN<Socket> s;
	Net::SocketUtil::AddressInfo addr;
	if (!sockf->DNSResolveIP(name, addr))
	{
		this->flags = 12;
		return;
	}
	if (addr.addrType == Net::AddrType::IPv4)
	{
		this->s = sockf->CreateTCPSocketv4();
		if (!this->s.SetTo(s))
		{
			this->flags = 12;
			return;
		}
	}
	else if (addr.addrType == Net::AddrType::IPv6)
	{
		this->s = sockf->CreateTCPSocketv6();
		if (!this->s.SetTo(s))
		{
			this->flags = 12;
			return;
		}
	}
	else
	{
		this->flags = 12;
		return;
	}
	if (!sockf->Connect(s, addr, port, timeout))
	{
		sockf->DestroySocket(s);
		this->s = 0;
		this->flags = 12;
		return;
	}
	this->cliId = sockf->GenSocketId(s);
}

Net::TCPClient::TCPClient(NN<Net::SocketFactory> sockf, UInt32 ip, UInt16 port, Data::Duration timeout) : IO::Stream(CSTR(""))
{
	this->currCnt = 0;
	this->s = 0;
	this->flags = 0;
	this->sockf = sockf;
	this->readEvent = 0;
	this->writeEvent = 0;
	this->timeout = 0;

/*	UTF8Char sbuff[32];
	UTF8Char *sptr;
	sptr = Net::SocketUtil::GetIPv4Name(sbuff, ip, port);
	this->SetSourceName(sbuff, (UOSInt)(sptr - sbuff));*/

	this->s = sockf->CreateTCPSocketv4();
	NN<Socket> s;
	if (!this->s.SetTo(s))
	{
		printf("Error in creating socket\r\n");
		this->flags = 12;
		return;
	}
	sockf->SetReuseAddr(s, true);
	if (!sockf->Connect(s, ip, port, timeout))
	{
		printf("Error in connecting\r\n");
		sockf->DestroySocket(s);
		this->s = 0;
		this->flags = 12;
		return;
	}
	this->cliId = sockf->GenSocketId(s);
}

Net::TCPClient::TCPClient(NN<Net::SocketFactory> sockf, NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::Duration timeout) : IO::Stream(CSTR(""))
{
	this->currCnt = 0;
	this->s = 0;
	this->flags = 0;
	this->sockf = sockf;
	this->readEvent = 0;
	this->writeEvent = 0;
	this->timeout = 0;

	NN<Socket> s;
	if (addr->addrType == Net::AddrType::IPv4)
	{
		this->s = sockf->CreateTCPSocketv4();
		if (!this->s.SetTo(s))
		{
#ifdef PRINTDEBUG
			IO::Console::PrintStrO((const UTF8Char*)"Error in creating TCP Socket v4\r\n");
#endif
			this->flags = 12;
			return;
		}
	}
	else if (addr->addrType == Net::AddrType::IPv6)
	{
		this->s = sockf->CreateTCPSocketv6();
		if (!this->s.SetTo(s))
		{
#ifdef PRINTDEBUG
			IO::Console::PrintStrO((const UTF8Char*)"Error in creating TCP Socket v6\r\n");
#endif
			this->flags = 12;
			return;
		}
	}
	else
	{
#ifdef PRINTDEBUG
		IO::Console::PrintStrO((const UTF8Char*)"Unknown address type\r\n");
#endif
		this->flags = 12;
		return;
	}
	if (!sockf->Connect(s, addr, port, timeout))
	{
#ifdef PRINTDEBUG
		IO::Console::PrintStrO((const UTF8Char*)"Cannot connect to destination\r\n");
#endif
		sockf->DestroySocket(s);
		this->s = 0;
		this->flags = 12;
		return;
	}
#ifdef PRINTDEBUG
	IO::Console::PrintStrO((const UTF8Char*)"TCPClient connected\r\n");
#endif
/*	UTF8Char sbuff[64];
	UTF8Char *sptr;
	sptr = sockf->GetRemoteName(sbuff, s);
	this->SetSourceName(sbuff, (UOSInt)(sptr - sbuff));*/
	this->cliId = sockf->GenSocketId(s);
}

Net::TCPClient::TCPClient(NN<Net::SocketFactory> sockf, Optional<Socket> s) : IO::Stream(CSTR(""))
{
	this->sockf = sockf;
	this->s = s;
	this->flags = 0;
	this->currCnt = 0;
	this->readEvent = 0;
	this->writeEvent = 0;
	this->timeout = 0;
	NN<Socket> nns;
	if (s.SetTo(nns))
	{
		this->cliId = sockf->GenSocketId(nns);
	/*
		UTF8Char sbuff[128];
		UTF8Char *sptr;
		sptr = sockf->GetRemoteName(sbuff, s);
		this->SetSourceName(sbuff, (UOSInt)(sptr - sbuff));*/
	}
	else
	{
		this->cliId = 0;
		this->flags = 4;
	}
}


Net::TCPClient::~TCPClient()
{
	Close();
}

Bool Net::TCPClient::IsDown() const
{
	if (this->s.IsNull() || (this->flags & 6) != 0)
	{
		return true;
	}
	return false;
}

UOSInt Net::TCPClient::Read(const Data::ByteArray &buff)
{
	NN<Socket> s;
	if (this->s.SetTo(s) && (this->flags & 6) == 0)
	{
		Net::SocketFactory::ErrorType et;
		UOSInt recvSize = sockf->ReceiveData(s, buff.Arr().Ptr(), buff.GetSize(), et);
		if (recvSize != 0)
		{
#ifdef PRINTDEBUG
			{
				Text::StringBuilderUTF8 sb;
				sb.AppendC(UTF8STRC("Client received "));
				sb.AppendUOSInt(recvSize);
				sb.AppendC(UTF8STRC(" bytes\r\n"));
				IO::Console::PrintStrO(sb.ToString());
			}
#endif
			this->currCnt += recvSize;
//			printf("%d byte read\r\n", recvSize);
			return recvSize;
		}
		else
		{
			if (et == Net::SocketFactory::ET_DISCONNECT)
			{
				this->flags |= 2;
			}
			else if (et == Net::SocketFactory::ET_SHUTDOWN)
			{
				this->flags |= 2;
			}
			else if (et == Net::SocketFactory::ET_NO_ERROR)
			{
			}
			else
			{
				this->Close();
			}
			return 0;
		}
	}
	else
	{
		return 0;
	}
}

UOSInt Net::TCPClient::Write(Data::ByteArrayR buff)
{
	NN<Socket> s;
	if (this->s.SetTo(s) && (this->flags & 5) == 0)
	{
		Net::SocketFactory::ErrorType et;
		UOSInt sendSize;
#ifdef PRINTDEBUG
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("Client sending "));
			sb.AppendUOSInt(buff.GetSize());
			sb.AppendC(UTF8STRC(" bytes out\r\n"));
			IO::Console::PrintStrO(sb.ToString());
		}
#endif
		sendSize = this->sockf->SendData(s, buff.Arr(), buff.GetSize(), et);
		if (sendSize > 0)
		{
			this->currCnt += sendSize;
			return sendSize;
		}
		else
		{
			if (et == Net::SocketFactory::ET_DISCONNECT)
			{
				this->flags |= 1;
			}
			else if (et == Net::SocketFactory::ET_SHUTDOWN)
			{
				this->flags |= 1;
			}
			else
			{
				this->Close();
			}
			return 0;
		}
	}
	else
	{
		return 0;
	}
}

void *Net::TCPClient::BeginRead(const Data::ByteArray &buff, Sync::Event *evt)
{
	NN<Socket> s;
	if (!this->s.SetTo(s) || (this->flags & 6) != 0)
		return 0;
	Net::SocketFactory::ErrorType et;
	void *data = sockf->BeginReceiveData(s, buff.Arr().Ptr(), buff.GetSize(), evt, et);
	if (data == 0)
	{
		if (et == Net::SocketFactory::ET_SHUTDOWN)
		{
			this->flags |= 2;
		}
		else if (et == Net::SocketFactory::ET_DISCONNECT)
		{
			this->flags |= 2;
		}
		else
		{
			this->Close();
		}
	}
	return data;
}

UOSInt Net::TCPClient::EndRead(void *reqData, Bool toWait, OutParam<Bool> incomplete)
{
	if (reqData == 0)
	{
		incomplete.Set(false);
		return 0;
	}
	return sockf->EndReceiveData(reqData, toWait, incomplete);
}

void Net::TCPClient::CancelRead(void *reqData)
{
	Bool incomplete;
	EndRead(reqData, true, incomplete);
}

void *Net::TCPClient::BeginWrite(Data::ByteArrayR buff, Sync::Event *evt)
{
	NN<Socket> s;
	if (!this->s.SetTo(s) || (this->flags & 5) != 0)
		return 0;
	void *data = (void*)Write(buff);
	if (data != 0 && evt != 0)
		evt->Set();
	return data;
}

UOSInt Net::TCPClient::EndWrite(void *reqData, Bool toWait)
{
	if (reqData == 0)
		return 0;
	return (UOSInt)reqData;
}

void Net::TCPClient::CancelWrite(void *reqData)
{
	EndWrite(reqData, true);
}

Int32 Net::TCPClient::Flush()
{
	return 0;
}

void Net::TCPClient::Close()
{
	if ((this->flags & 4) == 0)
	{
		this->flags |= 4;
		NN<Socket> s;
		if (this->s.SetTo(s))
		{
			this->sockf->DestroySocket(s);
			this->s = 0;
		}
	}
}

Bool Net::TCPClient::Recover()
{
	//////////////////////////////////////
	return false;
}

Bool Net::TCPClient::IsSSL() const
{
	return false;
}

IO::StreamType Net::TCPClient::GetStreamType() const
{
	return IO::StreamType::TCPClient;
}

Bool Net::TCPClient::IsClosed()
{
	return (this->flags & 4) != 0;
}

Bool Net::TCPClient::IsSendDown()
{
	return (this->flags & 1) != 0;
}

Bool Net::TCPClient::IsRecvDown()
{
	return (this->flags & 2) != 0;
}

Bool Net::TCPClient::IsConnectError()
{
	return (this->flags & 8) != 0;
}

UOSInt Net::TCPClient::GetRecvBuffSize()
{
	UInt32 argp;
	NN<Socket> s;
	if (this->s.SetTo(s) && this->sockf->SocketGetReadBuff(s, argp))
	{
		return argp;
	}
	else
	{
		return 0;
	}
}

Bool Net::TCPClient::Wait(Data::Duration dur)
{
	NN<Socket> s;
	return this->s.SetTo(s) && this->sockf->SocketWait(s, dur);
}

UInt64 Net::TCPClient::GetCliId()
{
	return this->cliId;
}

UnsafeArrayOpt<UTF8Char> Net::TCPClient::GetRemoteName(UnsafeArray<UTF8Char> buff) const
{
	UnsafeArray<UTF8Char> sptr;
	NN<Socket> s;
	if ((this->flags & 4) || !this->s.SetTo(s) || !this->sockf->GetRemoteName(buff, s).SetTo(sptr))
	{
		UInt32 ip;
		UInt16 port;
		Net::SocketFactory::FromSocketId(this->cliId, ip, port);
		return Net::SocketUtil::GetIPv4Name(buff, ip, port);
	}
	else
	{
		return sptr;
	}
}

UnsafeArrayOpt<UTF8Char> Net::TCPClient::GetLocalName(UnsafeArray<UTF8Char> buff) const
{
	NN<Socket> s;
	if (this->s.SetTo(s))
	{
		return this->sockf->GetLocalName(buff, s);
	}
	return 0;
}

Bool Net::TCPClient::GetRemoteAddr(NN<Net::SocketUtil::AddressInfo> addr) const
{
	NN<Socket> s;
	if (this->s.SetTo(s))
	{
		return this->sockf->GetRemoteAddr(s, addr, 0);
	}
	else
	{
		return false;
	}
}

UInt16 Net::TCPClient::GetRemotePort() const
{
	return (UInt16)((this->cliId >> 32) & 0xffff);
}

UInt16 Net::TCPClient::GetLocalPort() const
{
	return (UInt16)((this->cliId >> 48) & 0xffff);
}

void Net::TCPClient::SetNoDelay(Bool val)
{
	NN<Socket> s;
	if ((this->flags & 4) == 0 && this->s.SetTo(s))
	{
		this->sockf->SetNoDelay(s, val);
	}
}

void Net::TCPClient::ShutdownSend()
{
	NN<Socket> s;
	if ((this->flags & 4) == 0 && this->s.SetTo(s))
	{
		this->flags |= 1;
		this->sockf->ShutdownSend(s);
	}
}

void Net::TCPClient::SetTimeout(Data::Duration timeout)
{
	NN<Socket> s;
	if ((this->flags & 4) == 0 && this->s.SetTo(s))
	{
		this->sockf->SetRecvTimeout(s, timeout);
		this->timeout = timeout;
	}
}

Data::Duration Net::TCPClient::GetTimeout()
{
	return this->timeout;
}

Optional<Socket> Net::TCPClient::GetSocket()
{
	return s;
}

Optional<Socket> Net::TCPClient::RemoveSocket()
{
	Optional<Socket> s = this->s;
	this->s = 0;
	this->flags |= 4;
	return s;
}
