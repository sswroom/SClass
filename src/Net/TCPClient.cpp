#include "Stdafx.h"

#include "MyMemory.h"
#include "IO/Stream.h"
#include "Net/SocketFactory.h"
#include "Net/TCPClient.h"
#include "Sync/Event.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"

#include <stdio.h>
//#define PRINTDEBUG
#ifdef PRINTDEBUG
#include "IO/Console.h"
#include "Text/StringBuilderUTF8.h"
#endif

Net::TCPClient::TCPClient(Net::SocketFactory *sockf, Text::CString name, UInt16 port) : IO::Stream(name)
{
	this->currCnt = 0;
	this->flags = 0;
	s = 0;
	this->sockf = sockf;
	this->readEvent = 0;
	this->writeEvent = 0;
	this->timeoutMS = 0;

	Net::SocketUtil::AddressInfo addr;
	if (!sockf->DNSResolveIP(name, &addr))
	{
		this->flags = 12;
		return;
	}
	if (addr.addrType == Net::AddrType::IPv4)
	{
		s = sockf->CreateTCPSocketv4();
		if (s == 0)
		{
			this->flags = 12;
			return;
		}
	}
	else if (addr.addrType == Net::AddrType::IPv6)
	{
		s = sockf->CreateTCPSocketv6();
		if (s == 0)
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
	if (!sockf->Connect(s, &addr, port))
	{
		sockf->DestroySocket(s);
		s = 0;
		this->flags = 12;
		return;
	}
	this->cliId = sockf->GenSocketId(s);
}

Net::TCPClient::TCPClient(Net::SocketFactory *sockf, UInt32 ip, UInt16 port) : IO::Stream(CSTR(""))
{
	this->currCnt = 0;
	this->s = 0;
	this->flags = 0;
	this->sockf = sockf;
	this->readEvent = 0;
	this->writeEvent = 0;
	this->timeoutMS = 0;

/*	UTF8Char sbuff[32];
	UTF8Char *sptr;
	sptr = Net::SocketUtil::GetIPv4Name(sbuff, ip, port);
	this->SetSourceName(sbuff, (UOSInt)(sptr - sbuff));*/

	s = sockf->CreateTCPSocketv4();

	if (s == 0)
	{
		printf("Error in creating socket\r\n");
		this->flags = 12;
		return;
	}
	sockf->SetReuseAddr(s, true);
	if (!sockf->Connect(s, ip, port))
	{
		printf("Error in connecting\r\n");
		sockf->DestroySocket(s);
		s = 0;
		this->flags = 12;
		return;
	}
	this->cliId = sockf->GenSocketId(s);
}

Net::TCPClient::TCPClient(Net::SocketFactory *sockf, const Net::SocketUtil::AddressInfo *addr, UInt16 port) : IO::Stream(CSTR(""))
{
	this->currCnt = 0;
	this->s = 0;
	this->flags = 0;
	this->sockf = sockf;
	this->readEvent = 0;
	this->writeEvent = 0;
	this->timeoutMS = 0;

	if (addr->addrType == Net::AddrType::IPv4)
	{
		s = sockf->CreateTCPSocketv4();
		if (s == 0)
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
		s = sockf->CreateTCPSocketv6();
		if (s == 0)
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
	if (!sockf->Connect(s, addr, port))
	{
#ifdef PRINTDEBUG
		IO::Console::PrintStrO((const UTF8Char*)"Cannot connect to destination\r\n");
#endif
		sockf->DestroySocket(s);
		s = 0;
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

Net::TCPClient::TCPClient(Net::SocketFactory *sockf, Socket *s) : IO::Stream(CSTR(""))
{
	this->sockf = sockf;
	this->s = s;
	this->flags = 0;
	this->currCnt = 0;
	this->readEvent = 0;
	this->writeEvent = 0;
	this->timeoutMS = 0;
	if (s)
	{
		this->cliId = sockf->GenSocketId(s);
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
	if (this->s == 0 || (this->flags & 6) != 0)
	{
		return true;
	}
	return false;
}

UOSInt Net::TCPClient::Read(UInt8 *buff, UOSInt size)
{
	if (s && (this->flags & 6) == 0)
	{
		Net::SocketFactory::ErrorType et;
		UOSInt recvSize = sockf->ReceiveData(s, buff, size, &et);
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

UOSInt Net::TCPClient::Write(const UInt8 *buff, UOSInt size)
{
	if (s && (this->flags & 5) == 0)
	{
		Net::SocketFactory::ErrorType et;
		UOSInt sendSize;
#ifdef PRINTDEBUG
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("Client sending "));
			sb.AppendUOSInt(size);
			sb.AppendC(UTF8STRC(" bytes out\r\n"));
			IO::Console::PrintStrO(sb.ToString());
		}
#endif
		sendSize = this->sockf->SendData(s, buff, size, &et);
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

void *Net::TCPClient::BeginRead(UInt8 *buff, UOSInt size, Sync::Event *evt)
{
	if (s == 0 || (this->flags & 6) != 0)
		return 0;
	Net::SocketFactory::ErrorType et;
	void *data = sockf->BeginReceiveData(s, buff, size, evt, &et);
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

UOSInt Net::TCPClient::EndRead(void *reqData, Bool toWait, Bool *incomplete)
{
	if (reqData == 0)
	{
		*incomplete = false;
		return 0;
	}
	return sockf->EndReceiveData(reqData, toWait, incomplete);
}

void Net::TCPClient::CancelRead(void *reqData)
{
	Bool incomplete;
	EndRead(reqData, true, &incomplete);
}

void *Net::TCPClient::BeginWrite(const UInt8 *buff, UOSInt size, Sync::Event *evt)
{
	if (s == 0 || (this->flags & 5) != 0)
		return 0;
	void *data = (void*)Write(buff, size);
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
		this->sockf->DestroySocket(s);
	}
}

Bool Net::TCPClient::Recover()
{
	//////////////////////////////////////
	return false;
}

Bool Net::TCPClient::IsSSL()
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
	if (this->sockf->SocketGetReadBuff(this->s, &argp))
	{
		return argp;
	}
	else
	{
		return 0;
	}
}

UInt64 Net::TCPClient::GetCliId()
{
	return this->cliId;
}

UTF8Char *Net::TCPClient::GetRemoteName(UTF8Char *buff) const
{
	if (this->flags & 4)
	{
		UInt32 ip;
		UInt16 port;
		Net::SocketFactory::FromSocketId(this->cliId, &ip, &port);
		return Net::SocketUtil::GetIPv4Name(buff, ip, port);
	}
	else
	{
		return this->sockf->GetRemoteName(buff, this->s);
	}
}

UTF8Char *Net::TCPClient::GetLocalName(UTF8Char *buff) const
{
	return this->sockf->GetLocalName(buff, this->s);
}

Bool Net::TCPClient::GetRemoteAddr(Net::SocketUtil::AddressInfo *addr) const
{
	return this->sockf->GetRemoteAddr(this->s, addr, 0);
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
	this->sockf->SetNoDelay(this->s, val);
}

void Net::TCPClient::ShutdownSend()
{
	this->flags |= 1;
	this->sockf->ShutdownSend(this->s);
}

void Net::TCPClient::SetTimeout(Int32 ms)
{
	this->sockf->SetRecvTimeout(this->s, ms);
	this->timeoutMS = ms;
}

Int32 Net::TCPClient::GetTimeoutMS()
{
	return this->timeoutMS;
}

Socket *Net::TCPClient::GetSocket()
{
	return s;
}

Socket *Net::TCPClient::RemoveSocket()
{
	Socket *s = this->s;
	this->s = 0;
	this->flags |= 4;
	return s;
}
