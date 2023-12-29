#include "Stdafx.h"
#include "Crypto/Hash/CRC32R.h"
#include "Data/ByteTool.h"
#include "Data/FastMap.h"
#include "IO/Path.h"
#include "Manage/Process.h"
#include "Net/ConnectionInfo.h"
#include "Net/DNSHandler.h"
#include "Net/OSSocketFactory.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/UTF8Reader.h"
#include "Text/TextBinEnc/Punycode.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <mstcpip.h>
#ifndef _WIN32_WCE
#include <windns.h>

#include <iphlpapi.h>
#include <icmpapi.h>
#endif

#if defined(__CYGWIN__)
#define u_long __ms_u_long
#define timeval TIMEVAL
#endif
//#define VERBOSE
#if defined(VERBOSE)
#include <stdio.h>
#endif

struct Net::OSSocketFactory::ClassData
{
	Sync::Mutex socMut;
	Data::Int32FastMap<UInt8> acceptedSoc;
};

Bool Net::OSSocketFactory::MyConnect(Socket *socket, const UInt8 *addrBuff, UOSInt addrLen, Data::Duration timeout)
{
	int error = -1;
	int len = sizeof(int);
	timeval tm;
	fd_set set;
	u_long ul = 1;
	ioctlsocket((SOCKET)socket, FIONBIO, &ul);
	Bool ret = false;
	if (connect((SOCKET)socket, (struct sockaddr *)addrBuff, (int)addrLen) == -1)
	{
		tm.tv_sec = (long)timeout.GetSeconds();	// set the timeout. 10s
		tm.tv_usec = (long)timeout.GetNS() / 1000;
		FD_ZERO(&set);
		FD_SET((int)(OSInt)socket, &set);

		if (select((int)(OSInt)socket + 1, NULL, &set, NULL, &tm) > 0)
		{
			getsockopt((SOCKET)socket, SOL_SOCKET, SO_ERROR, (char *)&error, /*(socklen_t *)*/&len);
			if (error == 0)
				ret = true;
			else
				ret = false;
		}
		else
			ret = false;
	}
	else
		ret = true;
	
	ul = 0;
	ioctlsocket((SOCKET)socket, FIONBIO, &ul); //set as blocking
	return ret;
}

Net::OSSocketFactory::OSSocketFactory(Bool noV6DNS) : Net::SocketFactory(noV6DNS)
{
	WSADATA data;
	//Int32 iResult = WSAStartup(MAKEWORD(2, 2), &data);
	WSAStartup(MAKEWORD(2, 2), &data);
	this->toRelease = true;
	this->icmpHand = 0;
	NEW_CLASS(this->clsData, ClassData());
}

Net::OSSocketFactory::~OSSocketFactory()
{
	SDEL_CLASS(this->dnsHdlr);
	DEL_CLASS(this->clsData);
	if (this->toRelease)
	{
		WSACleanup();
		this->toRelease = false;
	}
	if (this->icmpHand)
	{
		IcmpCloseHandle((HANDLE)this->icmpHand);
	}
}

Socket *Net::OSSocketFactory::CreateTCPSocketv4()
{
	SOCKET s = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	if (s == INVALID_SOCKET)
		return 0;
	Sync::MutexUsage mutUsage(this->clsData->socMut);
	this->clsData->acceptedSoc.Put((Int32)s, 1);
	return (Socket*)s;
}

Socket *Net::OSSocketFactory::CreateTCPSocketv6()
{
	SOCKET s = WSASocket(AF_INET6, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	if (s == INVALID_SOCKET)
		return 0;
	Sync::MutexUsage mutUsage(this->clsData->socMut);
	this->clsData->acceptedSoc.Put((Int32)s, 1);
	return (Socket*)s;
}

Socket *Net::OSSocketFactory::CreateUDPSocketv4()
{
	SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (s == INVALID_SOCKET)
		return 0;
	return (Socket*)s;
}

Socket *Net::OSSocketFactory::CreateUDPSocketv6()
{
	SOCKET s = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
	if (s == INVALID_SOCKET)
		return 0;
	return (Socket*)s;
}

Socket *Net::OSSocketFactory::CreateICMPIPv4Socket(UInt32 ip)
{
	SOCKET s = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (s == INVALID_SOCKET)
		return 0;

	this->SocketBindv4((Socket*)s, ip, 0);
	int recv_all_opt = 1;
	int ioctl_read = 0;
	WSAIoctl(s, SIO_RCVALL, &recv_all_opt, sizeof(recv_all_opt), 0, 0, (LPDWORD)&ioctl_read, 0, 0);

	return (Socket*)s;
}

Socket *Net::OSSocketFactory::CreateUDPRAWv4Socket(UInt32 ip)
{
	SOCKET s = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
	if (s == INVALID_SOCKET)
		return 0;

	this->SocketBindv4((Socket*)s, ip, 0);
	int recv_all_opt = 1;
	int ioctl_read = 0;
	WSAIoctl(s, SIO_RCVALL, &recv_all_opt, sizeof(recv_all_opt), 0, 0, (LPDWORD)&ioctl_read, 0, 0);

	return (Socket*)s;
}

Socket *Net::OSSocketFactory::CreateRAWIPv4Socket(UInt32 ip)
{
	SOCKET s = socket(AF_INET, SOCK_RAW, IPPROTO_IP);
	if (s == INVALID_SOCKET)
		return 0;

	this->SocketBindv4((Socket*)s, ip, 0);
	int recv_all_opt = 1;
	int ioctl_read = 0;
	WSAIoctl(s, SIO_RCVALL, &recv_all_opt, sizeof(recv_all_opt), 0, 0, (LPDWORD)&ioctl_read, 0, 0);

	return (Socket*)s;
}

Socket *Net::OSSocketFactory::CreateARPSocket()
{
	return 0;
}

Socket *Net::OSSocketFactory::CreateRAWSocket()
{
	SOCKET s = socket(AF_UNSPEC, SOCK_RAW, IPPROTO_IP);
	if (s == INVALID_SOCKET)
		return 0;
	return (Socket*)s;
}

void Net::OSSocketFactory::DestroySocket(Socket *socket)
{
	Sync::MutexUsage mutUsage(this->clsData->socMut);
	this->clsData->acceptedSoc.Put((Int32)(OSInt)socket, 0);
	mutUsage.EndUse();
	closesocket((SOCKET)socket);
}

Bool Net::OSSocketFactory::SocketIsInvalid(Socket *socket)
{
	return (SOCKET)socket == INVALID_SOCKET;
}

Bool Net::OSSocketFactory::SocketBindv4(Socket *socket, UInt32 ip, UInt16 port)
{
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ip;
	addr.sin_port = htons(port);
	return bind((SOCKET)socket, (sockaddr*)&addr, sizeof(addr)) != SOCKET_ERROR;
}

Bool Net::OSSocketFactory::SocketBind(Socket *socket, const Net::SocketUtil::AddressInfo *addr, UInt16 port)
{
	if (addr == 0 || addr->addrType == Net::AddrType::IPv6)
	{
		UInt8 addrBuff[28];
		WriteInt16(&addrBuff[0], AF_INET6);
		WriteMInt16(&addrBuff[2], port); //sin6_port
		WriteMInt32(&addrBuff[4], 0); //sin6_flowinfo
		if (addr)
		{
			MemCopyNO(&addrBuff[8], addr->addr, 20);
		}
		else
		{
			MemClear(&addrBuff[8], 16);
			WriteMInt32(&addrBuff[24], 0); //sin6_scope_id
		}
		return bind((SOCKET)socket, (sockaddr*)addrBuff, 28) != SOCKET_ERROR;
	}
	else if (addr->addrType == Net::AddrType::IPv4)
	{
		sockaddr_in saddr;
		saddr.sin_family = AF_INET;
		saddr.sin_addr.s_addr = *(UInt32*)addr->addr;
		saddr.sin_port = htons(port);
		return bind((SOCKET)socket, (sockaddr*)&saddr, sizeof(saddr)) != SOCKET_ERROR;
	}
	else
	{
		return false;
	}
}

Bool Net::OSSocketFactory::SocketBindRAWIf(Socket *socket, UOSInt ifIndex)
{
	return false;
}

Bool Net::OSSocketFactory::SocketListen(Socket *socket)
{
	return listen((SOCKET)socket, 128) != SOCKET_ERROR;
}

Socket *Net::OSSocketFactory::SocketAccept(Socket *socket)
{
	UInt8 saddr[28];
	Int32 addrlen = 28;
//	sockaddr_in saddr;
//	Int32 addrlen = sizeof(saddr);
	SOCKET s;
#if defined(VERBOSE)
	UTF8Char debugBuff[64];
	Data::DateTime debugDt;
#endif
	while (true)
	{
		s = accept((SOCKET)socket, (sockaddr*)&saddr, &addrlen);
		if (s == INVALID_SOCKET)
		{
#if defined(VERBOSE)
			debugDt.SetCurrTime();
			debugDt.ToString(debugBuff, "HH:mm:ss.fff");
			printf("%s Accept invalid\r\n", debugBuff);
#endif
			return (Socket*)s;
		}
		Sync::MutexUsage mutUsage(this->clsData->socMut);
		if (this->clsData->acceptedSoc.Get((Int32)s) == 0)
		{
			this->clsData->acceptedSoc.Put((Int32)s, 2);
#if defined(VERBOSE)
			debugDt.SetCurrTime();
			debugDt.ToString(debugBuff, "HH:mm:ss.fff");
			printf("%s Socket Accepted\r\n", debugBuff);
#endif
			return (Socket*)s;
		}
#if defined(VERBOSE)
			debugDt.SetCurrTime();
			debugDt.ToString(debugBuff, "HH:mm:ss.fff");
			printf("%s Accept duplicated\r\n", debugBuff);
#endif
		mutUsage.EndUse();
	}
}

Int32 Net::OSSocketFactory::SocketGetLastError()
{
	return WSAGetLastError();
}

Bool Net::OSSocketFactory::GetRemoteAddr(Socket *socket, NotNullPtr<Net::SocketUtil::AddressInfo> addr, OptOut<UInt16> port)
{
	UInt8 addrBuff[28];
	int size = 28;
	if (getpeername((SOCKET)socket, (sockaddr*)addrBuff, &size) == 0)
	{
		if (*(Int16*)&addrBuff[0] == AF_INET)
		{
			addr->addrType = Net::AddrType::IPv4;
			*(UInt32*)addr->addr = ((sockaddr_in*)addrBuff)->sin_addr.S_un.S_addr;
			port.Set(ReadMUInt16(&addrBuff[2]));
			return true;
		}
		else if (*(Int16*)&addrBuff[0] == AF_INET6)
		{
			addr->addrType = Net::AddrType::IPv6;
			MemCopyNO(addr->addr, &addrBuff[8], 20);
			port.Set(ReadMUInt16(&addrBuff[2]));
			return true;
		}
	}
	return false;
}

Bool Net::OSSocketFactory::GetLocalAddr(Socket *socket, NotNullPtr<Net::SocketUtil::AddressInfo> addr, OptOut<UInt16> port)
{
	UInt8 addrBuff[28];
	int size = 28;
	if (getsockname((SOCKET)socket, (sockaddr*)addrBuff, &size) == 0)
	{
		if (*(Int16*)&addrBuff[0] == AF_INET)
		{
			addr->addrType = Net::AddrType::IPv4;
			*(UInt32*)addr->addr = ((sockaddr_in*)addrBuff)->sin_addr.S_un.S_addr;
			port.Set(ReadMUInt16(&addrBuff[2]));
			return true;
		}
		else if (*(Int16*)&addrBuff[0] == AF_INET6)
		{
			addr->addrType = Net::AddrType::IPv6;
			MemCopyNO(addr->addr, &addrBuff[8], 20);
			port.Set(ReadMUInt16(&addrBuff[2]));
			return true;
		}
	}
	return false;
}

OSInt Net::OSSocketFactory::SocketGetFD(Socket *socket)
{
	return (SOCKET)socket;
}

Bool Net::OSSocketFactory::SocketWait(Socket *socket, Data::Duration dur)
{
	fd_set fds;
	OSInt s = SocketGetFD(socket);
	FD_ZERO(&fds);
	FD_SET(s, &fds);
	timeval tv;
	tv.tv_sec = (time_t)dur.GetSeconds();
	tv.tv_usec = (long)(dur.GetNS() / 1000);
	int rc = select((int)(s + 1), &fds, NULL, NULL, &tv);
	if (rc == -1)
		return false;
	return rc != 0;
}

void Net::OSSocketFactory::SetDontLinger(Socket *socket, Bool val)
{
	BOOL ling;
	if (val)
	{
		ling = TRUE;
	}
	else
	{
		ling = FALSE;
	}
	Int32 ret = setsockopt((SOCKET)socket, SOL_SOCKET, SO_DONTLINGER, (const char*)&ling, sizeof(ling));
	if (ret != 0)
	{
		 ret = WSAGetLastError();
	}
}

void Net::OSSocketFactory::SetLinger(Socket *socket, UInt32 ms)
{
	linger ling;
	ling.l_onoff = 1;
	ling.l_linger = (UInt16)(ms / 1000);
//	Int32 ret = setsockopt((SOCKET)socket, SOL_SOCKET, SO_LINGER, (const char*)&ling, sizeof(ling));
	setsockopt((SOCKET)socket, SOL_SOCKET, SO_LINGER, (const char*)&ling, sizeof(ling));
}

void Net::OSSocketFactory::SetRecvBuffSize(Socket *socket, Int32 buffSize)
{
	setsockopt((SOCKET)socket, SOL_SOCKET, SO_RCVBUF, (const char*)&buffSize, sizeof(Int32));
}

void Net::OSSocketFactory::SetNoDelay(Socket *socket, Bool val)
{
	BOOL v = val;
	setsockopt((SOCKET)socket, IPPROTO_TCP, TCP_NODELAY, (char*)&v, sizeof(v));
}

void Net::OSSocketFactory::SetSendTimeout(Socket *socket, Data::Duration timeout)
{
	DWORD ms = (DWORD)timeout.GetTotalMS(); 
	setsockopt((SOCKET)socket, SOL_SOCKET, SO_SNDTIMEO, (Char*)&ms, sizeof(ms));
}

void Net::OSSocketFactory::SetRecvTimeout(Socket *socket, Data::Duration timeout)
{
	DWORD ms = (DWORD)timeout.GetTotalMS(); 
	setsockopt((SOCKET)socket, SOL_SOCKET, SO_RCVTIMEO, (Char*)&ms, sizeof(ms));
}

void Net::OSSocketFactory::SetReuseAddr(Socket *socket, Bool val)
{
	BOOL v = val;
	setsockopt((SOCKET)socket, SOL_SOCKET, SO_REUSEADDR, (char*)&v, sizeof(v));
}

void Net::OSSocketFactory::SetIPv4TTL(Socket *socket, Int32 ttl)
{
	setsockopt((SOCKET)socket, IPPROTO_IP, IP_TTL, (char*)&ttl, sizeof(ttl));
}

void Net::OSSocketFactory::SetBroadcast(Socket *socket, Bool val)
{
	BOOL v = val;
	setsockopt((SOCKET)socket, SOL_SOCKET, SO_BROADCAST, (char*)&v, sizeof(v));
}

void Net::OSSocketFactory::AddIPMembership(Socket *socket, UInt32 ip)
{
	struct ip_mreq mreq;
	mreq.imr_multiaddr.s_addr = ip;
	mreq.imr_interface.s_addr = 0;
	setsockopt((SOCKET)socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq));
}

UOSInt Net::OSSocketFactory::SendData(Socket *socket, const UInt8 *buff, UOSInt buffSize, OptOut<ErrorType> et)
{
//	return send((SOCKET)socket, (const char*)buff, (int)buffSize, 0);
	WSABUF buf;
	DWORD sendSize;
	DWORD flags = 0;
	Int32 iResult;
	buf.buf = (char*)buff;
	buf.len = (ULONG)buffSize;
	iResult = WSASend((SOCKET)socket, &buf, 1, &sendSize, flags, 0, 0);
	if (iResult == 0)
	{
		et.Set(Net::SocketFactory::ET_NO_ERROR);
		return sendSize;
	}
	if (et.IsNotNull())
	{
		et.SetNoCheck(FromSystemError(WSAGetLastError()));
	}
	return 0;
}

UOSInt Net::OSSocketFactory::ReceiveData(Socket *socket, UInt8 *buff, UOSInt buffSize, OptOut<ErrorType> et)
{
	WSABUF buf;
	DWORD flags = 0;
	DWORD recvSize;
	Int32 iResult;
	buf.buf = (char*)buff;
	buf.len = (ULONG)buffSize;
	iResult = WSARecv((SOCKET)socket, &buf, 1, &recvSize, &flags, 0, 0);
	if (iResult == 0)
	{
		et.Set(Net::SocketFactory::ET_NO_ERROR);
		return recvSize;
	}
	if (et.IsNotNull())
	{
		et.SetNoCheck(FromSystemError(WSAGetLastError()));
	}
	return 0;
}

typedef struct
{
	WSAOVERLAPPED overlapped;
	DWORD flags;
	DWORD recvSize;
	WSABUF buf;
	Socket *s;
} WSAOverlapped;

void *Net::OSSocketFactory::BeginReceiveData(Socket *socket, UInt8 *buff, UOSInt buffSize, Sync::Event *evt, OptOut<ErrorType> et)
{
	WSAOverlapped *overlapped;
	overlapped = MemAlloc(WSAOverlapped, 1);
	overlapped->flags = 0;
	overlapped->recvSize = 0;
	overlapped->buf.buf = (char*)buff;
	overlapped->buf.len = (ULONG)buffSize;
	overlapped->s = socket;
	MemClear(&overlapped->overlapped, sizeof(WSAOVERLAPPED));
	overlapped->overlapped.hEvent = evt->GetHandle();
	Int32 iResult = WSARecv((SOCKET)socket, &overlapped->buf, 1, &overlapped->recvSize, &overlapped->flags, &overlapped->overlapped, 0);
	if ((iResult == SOCKET_ERROR))
	{
		Int32 lastErr = WSAGetLastError();
		if (WSA_IO_PENDING != lastErr)
		{
			Int32 err = WSAGetLastError();
			if (err != WSA_IO_PENDING)
			{
				if (et.IsNotNull())
				{
					et.SetNoCheck(FromSystemError(err));
				}
				MemFree(overlapped);
				return 0;
			}
		}
	}
	return overlapped;
}

UOSInt Net::OSSocketFactory::EndReceiveData(void *reqData, Bool toWait, OutParam<Bool> incomplete)
{
	WSAOverlapped *overlapped = (WSAOverlapped*)reqData;
	incomplete.Set(false);
	if (overlapped == 0)
		return 0;
	DWORD recvSize;
	DWORD flags;
	if (WSAGetOverlappedResult((SOCKET)overlapped->s, &overlapped->overlapped, &recvSize, toWait?TRUE:FALSE, &flags))
	{
		MemFree(overlapped);
		return recvSize;
	}
	else
	{
		int lastErr = WSAGetLastError();
		if (lastErr == WSA_IO_INCOMPLETE)
		{
			incomplete.Set(true);
			return 0;
		}
		MemFree(overlapped);
		return 0;
	}
}

void Net::OSSocketFactory::CancelReceiveData(void *reqData)
{
	WSAOverlapped *overlapped = (WSAOverlapped*)reqData;
	if (overlapped == 0)
		return;
#ifndef _WIN32_WCE
	CancelIo((HANDLE)overlapped->s);
#endif
	MemFree(overlapped);
}

UOSInt Net::OSSocketFactory::UDPReceive(Socket *socket, UInt8 *buff, UOSInt buffSize, NotNullPtr<Net::SocketUtil::AddressInfo> addr, OutParam<UInt16> port, OptOut<ErrorType> et)
{
	Int32 recvSize;
	UInt8 addrBuff[28];
	Int32 addrSize = 28;
	recvSize = recvfrom((SOCKET)socket, (Char*)buff, (int)buffSize, 0, (sockaddr*)addrBuff, &addrSize);
	if (recvSize <= 0)
	{
		if (et.IsNotNull())
		{
			et.SetNoCheck(FromSystemError(WSAGetLastError()));
		}
		return 0;
	}
	else
	{
		if (*(Int16*)&addrBuff[0] == AF_INET)
		{
			addr->addrType = Net::AddrType::IPv4;
			*(Int32*)addr->addr = *(Int32*)&addrBuff[4];
			port.Set(ReadMUInt16(&addrBuff[2]));
		}
		else if (*(Int16*)&addrBuff[0] == AF_INET6)
		{
			addr->addrType = Net::AddrType::IPv6;
			MemCopyNO(addr->addr, &addrBuff[8], 20);
			port.Set(ReadMUInt16(&addrBuff[2]));
		}
		return (UOSInt)recvSize;
	}
}

UOSInt Net::OSSocketFactory::SendTo(Socket *socket, const UInt8 *buff, UOSInt buffSize, NotNullPtr<const Net::SocketUtil::AddressInfo> addr, UInt16 port)
{
	UInt8 addrBuff[28];
	if (addr->addrType == Net::AddrType::IPv4)
	{
		*(Int16*)&addrBuff[0] = AF_INET;
		WriteMInt16(&addrBuff[2], port);
		*(Int32*)&addrBuff[4] = *(Int32*)addr->addr;
		int ret = sendto((SOCKET)socket, (const char*)buff, (int)buffSize, 0, (sockaddr*)addrBuff, sizeof(sockaddr_in));
		if (ret == SOCKET_ERROR)
		{
			return 0;
		}
		else
		{
			return (UOSInt)ret;
		}
	}
	else if (addr->addrType == Net::AddrType::IPv6)
	{
		*(Int16*)&addrBuff[0] = AF_INET6;
		WriteMInt16(&addrBuff[2], port);
		WriteMInt32(&addrBuff[4], 0);
		MemCopyNO(&addrBuff[8], addr->addr, 20);
		int ret = sendto((SOCKET)socket, (const char*)buff, (int)buffSize, 0, (sockaddr*)addrBuff, 28);
		if (ret == SOCKET_ERROR)
		{
			return 0;
		}
		else
		{
			return (UOSInt)ret;
		}
	}
	return 0;
}

UOSInt Net::OSSocketFactory::SendToIF(Socket *socket, const UInt8 *buff, UOSInt buffSize, const UTF8Char *ifName)
{
	return 0;
}

Bool Net::OSSocketFactory::IcmpSendEcho2(NotNullPtr<const Net::SocketUtil::AddressInfo> addr, UInt32 *respTime_us, UInt32 *ttl)
{
	UInt8 sendData[32];
	UInt8 replyBuff[sizeof(ICMP_ECHO_REPLY) + sizeof(sendData)];
	PICMP_ECHO_REPLY reply;

	UInt32 dwRetVal = 0;
	if (addr->addrType == Net::AddrType::IPv4)
	{
		if (this->icmpHand == 0)
		{
			this->icmpHand = IcmpCreateFile();
		}
		dwRetVal = ::IcmpSendEcho((HANDLE)this->icmpHand, *(UInt32*)addr->addr, sendData, sizeof(sendData), NULL, replyBuff, sizeof(replyBuff), 1000);
	}
    if (dwRetVal != 0)
	{
        reply = (PICMP_ECHO_REPLY)replyBuff;
		*respTime_us = reply->RoundTripTime * 1000;
		*ttl = reply->Options.Ttl;
		return true;
    }
    else
	{
        return false;
    }
}

Bool Net::OSSocketFactory::Connect(Socket *socket, UInt32 ip, UInt16 port, Data::Duration timeout)
{
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ip;
	addr.sin_port = htons(port);
	return MyConnect(socket, (const UInt8*)&addr, sizeof(addr), timeout);
}

Bool Net::OSSocketFactory::Connect(Socket *socket, NotNullPtr<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::Duration timeout)
{
	UInt8 addrBuff[28];
	if (addr->addrType == Net::AddrType::IPv4)
	{
		*(Int16*)&addrBuff[0] = AF_INET;
		WriteMInt16(&addrBuff[2], port);
		*(Int32*)&addrBuff[4] = *(Int32*)addr->addr;
		return MyConnect(socket, addrBuff, sizeof(sockaddr_in), timeout);
	}
	else if (addr->addrType == Net::AddrType::IPv6)
	{
		*(Int16*)&addrBuff[0] = AF_INET6;
		WriteMInt16(&addrBuff[2], port);
		WriteMInt32(&addrBuff[4], 0);
		MemCopyNO(&addrBuff[8], addr->addr, 20);
		return MyConnect(socket, addrBuff, 28, timeout);
	}
	else
	{
		return false;
	}
}


void Net::OSSocketFactory::ShutdownSend(Socket *socket)
{
	shutdown((SOCKET)socket, SD_SEND);
}

void Net::OSSocketFactory::ShutdownSocket(Socket *socket)
{
	shutdown((SOCKET)socket, SD_BOTH);
}

Bool Net::OSSocketFactory::SocketGetReadBuff(Socket *socket, UInt32 *size)
{
#if defined(__CYGWIN__)
	__ms_u_long argp;
#else
	u_long argp;
#endif
	int ret = ioctlsocket((SOCKET)socket, FIONREAD, &argp);
	*size = argp;
	return ret == 0;
}


Bool Net::OSSocketFactory::DNSResolveIPDef(const Char *host, NotNullPtr<Net::SocketUtil::AddressInfo> addr)
{
	Bool succ = false;
	addrinfo *result = 0;
	Int32 iResult = getaddrinfo(host, 0, 0, &result);
	if (iResult == 0)
	{
		if (result->ai_addr->sa_family ==AF_INET)
		{
			addr->addrType = Net::AddrType::IPv4;
			*(UInt32*)&addr->addr = ((sockaddr_in*)result->ai_addr)->sin_addr.S_un.S_addr;
			succ = true;
		}
		else if (result->ai_addr->sa_family == AF_INET6)
		{
			addr->addrType = Net::AddrType::IPv6;
			MemCopyNO(addr->addr, &result->ai_addr->sa_data[6], 20);
			succ = true;
		}
		freeaddrinfo(result);
		return succ;
	}
	else
	{
		return 0;
	}
}

Bool Net::OSSocketFactory::GetDefDNS(NotNullPtr<Net::SocketUtil::AddressInfo> addr)
{
#ifdef _WIN32_WCE
	/////////////////////////////////
/*HKEY_LOCAL_MACHINE
      \Comm
           \*Adapter Name*
                \Parameters:
                     \Tcpip
                         \DNS */
	return false;
#else
	UInt8 buff[256];
	DWORD buffSize;
	buffSize = 256;
	if (DnsQueryConfig(DnsConfigDnsServerList, 0, 0, 0, buff, &buffSize) == 0)
	{
		IP4_ARRAY *arr = (IP4_ARRAY*)buff;
		Net::SocketUtil::SetAddrInfoV4(addr, arr->AddrArray[0]);
		return true;
	}
	else
	{
		UInt32 defDNS = 0;
		UInt32 defGW;
		UOSInt i;
		Net::ConnectionInfo *connInfo;
		Data::ArrayList<Net::ConnectionInfo*> connInfos;
		if (this->GetConnInfoList(&connInfos))
		{
			i = connInfos.GetCount();
			while (i-- > 0)
			{
				connInfo = connInfos.GetItem(i);

				defGW = connInfo->GetDefaultGW();
				if (defGW != 0)
				{
					defDNS = connInfo->GetDNSAddress(0);
				}
				DEL_CLASS(connInfo);
			}
			Net::SocketUtil::SetAddrInfoV4(addr, defDNS);
			return true;
		}
		else
		{
			return false;
		}
	}
#endif
}

UOSInt Net::OSSocketFactory::GetDNSList(Data::ArrayList<UInt32> *dnsList)
{
#ifdef _WIN32_WCE
	/////////////////////////////////
/*HKEY_LOCAL_MACHINE
      \Comm
           \*Adapter Name*
                \Parameters:
                     \Tcpip
                         \DNS */
	return 0;
#else
	UInt8 buff[256];
	DWORD buffSize;
	UOSInt i;
	UOSInt j;
	buffSize = 256;
	if (DnsQueryConfig(DnsConfigDnsServerList, 0, 0, 0, buff, &buffSize) == 0)
	{
		IP4_ARRAY *arr = (IP4_ARRAY*)buff;
		i = 0;
		j = arr->AddrCount;
		while (i < j)
		{
			dnsList->Add(arr->AddrArray[i]);
			i++;
		}
		return j;
	}
	else
	{
		UInt32 ip;
		UInt32 defGW;
		Net::ConnectionInfo *connInfo;
		Data::ArrayList<Net::ConnectionInfo*> connInfos;
		if (this->GetConnInfoList(&connInfos))
		{
			j = 0;
			i = connInfos.GetCount();
			while (i-- > 0 && j == 0)
			{
				connInfo = connInfos.GetItem(i);

				defGW = connInfo->GetDefaultGW();
				if (defGW != 0)
				{
					j = 0;
					while ((ip = connInfo->GetDNSAddress(j)) != 0)
					{
						dnsList->Add(ip);
						j++;
					}
				}
				DEL_CLASS(connInfo);
			}
			return j;
		}
		else
		{
			return 0;
		}
	}
#endif
}

Bool Net::OSSocketFactory::LoadHosts(Net::DNSHandler *dnsHdlr)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr = IO::Path::GetOSPath(sbuff);
	sptr = Text::StrConcatC(sptr, UTF8STRC("\\System32\\drivers\\etc\\hosts"));
	Net::SocketUtil::AddressInfo addr;
	UOSInt i;
	Text::PString sarr[2];
	IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (fs.IsError())
	{
		return false;
	}
	Text::UTF8Reader reader(fs);
	Text::StringBuilderUTF8 sb;
	while (reader.ReadLine(sb, 512))
	{
		sb.Trim();
		if (sb.ToString()[0] == '#')
		{
		}
		else
		{
			i = Text::StrSplitWSP(sarr, 2, sb);
			if (i == 2)
			{
				if (Net::SocketUtil::SetAddrInfo(addr, sarr[0].ToCString()))
				{
					while (true)
					{
						i = Text::StrSplitWSP(sarr, 2, sarr[1]);
						dnsHdlr->AddHost(addr, sarr[0].v, sarr[0].leng);
						if (i != 2)
							break;
					}
				}
			}
		}
		
		sb.ClearStr();
	}
	return true;
}

Bool Net::OSSocketFactory::ARPAddRecord(UOSInt ifIndex, const UInt8 *hwAddr, UInt32 ipv4)
{
#if defined(__MINGW32__) || defined(__CYGWIN__)
	MIB_IPNETROW row;
	row.dwIndex = (UInt32)ifIndex;
	row.dwPhysAddrLen = 6;
	MemCopyNO(row.bPhysAddr, hwAddr, 6);
	row.dwAddr = ipv4;
	row.dwType = MIB_IPNET_TYPE_DYNAMIC;
	return 0 == CreateIpNetEntry(&row);
#else
	MIB_IPNETROW_LH row;
	row.dwIndex = (UInt32)ifIndex;
	row.dwPhysAddrLen = 6;
	MemCopyNO(row.bPhysAddr, hwAddr, 6);
	row.dwAddr = ipv4;
	row.dwType = MIB_IPNET_TYPE_DYNAMIC;
	return 0 == CreateIpNetEntry(&row);
#endif
}


UOSInt Net::OSSocketFactory::GetConnInfoList(Data::ArrayList<Net::ConnectionInfo*> *connInfoList)
{
	UInt32 size = 0;
	UOSInt nAdapters = 0;
	IP_ADAPTER_ADDRESSES *addrs;
	IP_ADAPTER_ADDRESSES *addr;
	IP_ADAPTER_INFO *infos;
	IP_ADAPTER_INFO *info;


	if (ERROR_BUFFER_OVERFLOW == GetAdaptersAddresses(AF_INET, 0, 0, 0, (PULONG)&size))
	{
		addrs = (IP_ADAPTER_ADDRESSES*)MAlloc(size);

		if (ERROR_SUCCESS == GetAdaptersAddresses(AF_INET, 0, 0, addrs, (PULONG)&size))
		{
			addr = addrs;
			while (addr)
			{
				Net::ConnectionInfo *connInfo;
				NEW_CLASS(connInfo, Net::ConnectionInfo(addr));
				connInfoList->Add(connInfo);
				nAdapters++;

				addr = addr->Next;
			}
		}

		MemFree(addrs);
	}

	size = 0;
	if (ERROR_BUFFER_OVERFLOW == GetAdaptersInfo(0, (PULONG)&size))
	{
		infos = (IP_ADAPTER_INFO*)MAlloc(size);

		if (ERROR_SUCCESS == GetAdaptersInfo(infos, (PULONG)&size))
		{
			UOSInt i = 0;
			info = infos;
			while (info)
			{
				Net::ConnectionInfo *connInfo;
				i = 0;
				while ((connInfo = connInfoList->GetItem(i++)) != 0)
				{
					if (connInfo->SetInfo(info))
						break;
				}

				info = info->Next;
			}
		}

		MemFree(infos);
	}
	return nAdapters;
}

Bool Net::OSSocketFactory::GetIPInfo(NotNullPtr<IPInfo> info)
{
	return GetIpStatisticsEx((MIB_IPSTATS*)info.Ptr(), AF_INET) == NO_ERROR;
}

Bool Net::OSSocketFactory::GetTCPInfo(NotNullPtr<TCPInfo> info)
{
	return GetTcpStatisticsEx((MIB_TCPSTATS*)info.Ptr(), AF_INET) == NO_ERROR;
}

Bool Net::OSSocketFactory::GetUDPInfo(NotNullPtr<UDPInfo> info)
{
	return GetUdpStatisticsEx((MIB_UDPSTATS*)info.Ptr(), AF_INET) == NO_ERROR;
}

UOSInt Net::OSSocketFactory::QueryPortInfos(Data::ArrayList<PortInfo*> *portInfoList, ProtocolType protoType, UInt16 procId)
{
	UOSInt retCnt = 0;
	UInt32 ret;
	if (protoType & Net::SocketFactory::PT_TCP)
	{
		Net::SocketFactory::PortInfo *port;
		MIB_TCPTABLE *tcpTable;
		UInt32 dwSize = 0;
		ret = GetTcpTable(0, (ULONG*)&dwSize, TRUE);
		if (ret == ERROR_INSUFFICIENT_BUFFER)
		{
			tcpTable = (MIB_TCPTABLE*)MAlloc(dwSize);
			ret = GetTcpTable(tcpTable, (ULONG*)&dwSize, TRUE);

			if (ret == NO_ERROR)
			{
				dwSize = 0;
				while (dwSize < tcpTable->dwNumEntries)
				{
					port = MemAlloc(Net::SocketFactory::PortInfo, 1);
					port->protoType = Net::SocketFactory::PT_TCP;
					port->localIP = tcpTable->table[dwSize].dwLocalAddr;
					port->localPort = tcpTable->table[dwSize].dwLocalPort;
					port->foreignIP = tcpTable->table[dwSize].dwRemoteAddr;
					port->foreignPort = tcpTable->table[dwSize].dwRemotePort;
					switch (tcpTable->table[dwSize].dwState)
					{
					case MIB_TCP_STATE_CLOSED:
						port->portState = Net::SocketFactory::PS_CLOSED;
						break;
					case MIB_TCP_STATE_LISTEN:
						port->portState = Net::SocketFactory::PS_LISTEN;
						break;
					case MIB_TCP_STATE_SYN_SENT:
						port->portState = Net::SocketFactory::PS_SYN_SENT;
						break;
					case MIB_TCP_STATE_SYN_RCVD:
						port->portState = Net::SocketFactory::PS_SYN_RCVD;
						break;
					case MIB_TCP_STATE_ESTAB:
						port->portState = Net::SocketFactory::PS_ESTAB;
						break;
					case MIB_TCP_STATE_FIN_WAIT1:
						port->portState = Net::SocketFactory::PS_FIN_WAIT1;
						break;
					case MIB_TCP_STATE_FIN_WAIT2:
						port->portState = Net::SocketFactory::PS_FIN_WAIT2;
						break;
					case MIB_TCP_STATE_CLOSE_WAIT:
						port->portState = Net::SocketFactory::PS_CLOSE_WAIT;
						break;
					case MIB_TCP_STATE_CLOSING:
						port->portState = Net::SocketFactory::PS_CLOSING;
						break;
					case MIB_TCP_STATE_LAST_ACK:
						port->portState = Net::SocketFactory::PS_LAST_ACK;
						break;
					case MIB_TCP_STATE_TIME_WAIT:
						port->portState = Net::SocketFactory::PS_TIME_WAIT;
						break;
					case MIB_TCP_STATE_DELETE_TCB:
						port->portState = Net::SocketFactory::PS_DELETE_TCB;
						break;
					default:
						port->portState = Net::SocketFactory::PS_UNKNOWN;
						break;
					}
					port->processId = 0;
					portInfoList->Add(port);
					dwSize++;
				}
				retCnt += tcpTable->dwNumEntries;
			}
			MemFree(tcpTable);
		}
	}
	if (protoType & Net::SocketFactory::PT_UDP)
	{
		Net::SocketFactory::PortInfo *port;
		MIB_UDPTABLE *udpTable;
		UInt32 dwSize = 0;
		ret = GetUdpTable(0, (ULONG*)&dwSize, TRUE);
		if (ret == ERROR_INSUFFICIENT_BUFFER)
		{
			udpTable = (MIB_UDPTABLE*)MAlloc(dwSize);
			ret = GetUdpTable(udpTable, (ULONG*)&dwSize, TRUE);

			if (ret == NO_ERROR)
			{
				dwSize = 0;
				while (dwSize < udpTable->dwNumEntries)
				{
					port = MemAlloc(Net::SocketFactory::PortInfo, 1);
					port->protoType = Net::SocketFactory::PT_UDP;
					port->localIP = udpTable->table[dwSize].dwLocalAddr;
					port->localPort = udpTable->table[dwSize].dwLocalPort;
					port->foreignIP = 0;
					port->foreignPort = 0;
					port->portState = Net::SocketFactory::PS_UNKNOWN;
					port->processId = 0;
					portInfoList->Add(port);
					dwSize++;
				}
				retCnt += udpTable->dwNumEntries;
			}
			MemFree(udpTable);
		}
	}
	return retCnt;
}

void Net::OSSocketFactory::FreePortInfos(Data::ArrayList<Net::SocketFactory::PortInfo*> *portInfoList)
{
	UOSInt i = portInfoList->GetCount();
	while (i-- > 0)
	{
		MemFree(portInfoList->RemoveAt(i));
	}
}

UOSInt Net::OSSocketFactory::QueryPortInfos2(Data::ArrayList<Net::SocketFactory::PortInfo3*> *portInfoList, ProtocolType protoType, UInt16 procId)
{
	UOSInt retCnt = 0;
	UInt32 ret;
	if (protoType & Net::SocketFactory::PT_TCP)
	{
		Net::SocketFactory::PortInfo3 *port;
		MIB_TCPTABLE *tcpTable;
		UInt32 dwSize = 0;
		ret = GetTcpTable(0, (ULONG*)&dwSize, TRUE);
		if (ret == ERROR_INSUFFICIENT_BUFFER)
		{
			tcpTable = (MIB_TCPTABLE*)MAlloc(dwSize);
			ret = GetTcpTable(tcpTable, (ULONG*)&dwSize, TRUE);

			if (ret == NO_ERROR)
			{
				dwSize = 0;
				while (dwSize < tcpTable->dwNumEntries)
				{
					port = MemAlloc(Net::SocketFactory::PortInfo3, 1);
					port->protoType = Net::SocketFactory::PT_TCP;
					Net::SocketUtil::SetAddrInfoV4(port->localAddr,  tcpTable->table[dwSize].dwLocalAddr);
					port->localPort = tcpTable->table[dwSize].dwLocalPort;
					Net::SocketUtil::SetAddrInfoV4(port->foreignAddr, tcpTable->table[dwSize].dwRemoteAddr);
					port->foreignPort = tcpTable->table[dwSize].dwRemotePort;
					switch (tcpTable->table[dwSize].dwState)
					{
					case MIB_TCP_STATE_CLOSED:
						port->portState = Net::SocketFactory::PS_CLOSED;
						break;
					case MIB_TCP_STATE_LISTEN:
						port->portState = Net::SocketFactory::PS_LISTEN;
						break;
					case MIB_TCP_STATE_SYN_SENT:
						port->portState = Net::SocketFactory::PS_SYN_SENT;
						break;
					case MIB_TCP_STATE_SYN_RCVD:
						port->portState = Net::SocketFactory::PS_SYN_RCVD;
						break;
					case MIB_TCP_STATE_ESTAB:
						port->portState = Net::SocketFactory::PS_ESTAB;
						break;
					case MIB_TCP_STATE_FIN_WAIT1:
						port->portState = Net::SocketFactory::PS_FIN_WAIT1;
						break;
					case MIB_TCP_STATE_FIN_WAIT2:
						port->portState = Net::SocketFactory::PS_FIN_WAIT2;
						break;
					case MIB_TCP_STATE_CLOSE_WAIT:
						port->portState = Net::SocketFactory::PS_CLOSE_WAIT;
						break;
					case MIB_TCP_STATE_CLOSING:
						port->portState = Net::SocketFactory::PS_CLOSING;
						break;
					case MIB_TCP_STATE_LAST_ACK:
						port->portState = Net::SocketFactory::PS_LAST_ACK;
						break;
					case MIB_TCP_STATE_TIME_WAIT:
						port->portState = Net::SocketFactory::PS_TIME_WAIT;
						break;
					case MIB_TCP_STATE_DELETE_TCB:
						port->portState = Net::SocketFactory::PS_DELETE_TCB;
						break;
					default:
						port->portState = Net::SocketFactory::PS_UNKNOWN;
						break;
					}
					port->processId = 0;
					port->socketId = 0;
					portInfoList->Add(port);
					dwSize++;
				}
				retCnt += tcpTable->dwNumEntries;
			}
			MemFree(tcpTable);
		}
	}
	if (protoType & Net::SocketFactory::PT_UDP)
	{
		Net::SocketFactory::PortInfo3 *port;
		MIB_UDPTABLE *udpTable;
		UInt32 dwSize = 0;
		ret = GetUdpTable(0, (ULONG*)&dwSize, TRUE);
		if (ret == ERROR_INSUFFICIENT_BUFFER)
		{
			udpTable = (MIB_UDPTABLE*)MAlloc(dwSize);
			ret = GetUdpTable(udpTable, (ULONG*)&dwSize, TRUE);

			if (ret == NO_ERROR)
			{
				dwSize = 0;
				while (dwSize < udpTable->dwNumEntries)
				{
					port = MemAlloc(Net::SocketFactory::PortInfo3, 1);
					port->protoType = Net::SocketFactory::PT_UDP;
					Net::SocketUtil::SetAddrInfoV4(port->localAddr, udpTable->table[dwSize].dwLocalAddr);
					port->localPort = udpTable->table[dwSize].dwLocalPort;
					Net::SocketUtil::SetAddrInfoV4(port->foreignAddr, 0);
					port->foreignPort = 0;
					port->portState = Net::SocketFactory::PS_UNKNOWN;
					port->processId = 0;
					port->socketId = 0;
					portInfoList->Add(port);
					dwSize++;
				}
				retCnt += udpTable->dwNumEntries;
			}
			MemFree(udpTable);
		}
	}
	return retCnt;
}

void Net::OSSocketFactory::FreePortInfos2(Data::ArrayList<Net::SocketFactory::PortInfo3*> *portInfoList)
{
	UOSInt i = portInfoList->GetCount();
	while (i-- > 0)
	{
		MemFree(portInfoList->RemoveAt(i));
	}
}

/*UInt32 Net::SocketFactory::GetLocalIPByDest(const WChar *host)
{
	UInt32 destIP = GetIPv4ByHost(host);
	return GetLocalIPByDest(destIP);
}

UInt32 Net::SocketFactory::GetLocalIPByDest(UInt32 ip)
{
	UInt32 defIP = 0;
	UInt32 defGW;
	UInt32 currIP;
	UInt32 netMask;
	UInt32 foundIP = 0;
	OSInt i;
	OSInt j;
	Net::ConnectionInfo *connInfo;
	Data::ArrayList<Net::ConnectionInfo*> connInfos;
	if (Net::ConnectionInfo::GetConnInfoList(&connInfos))
	{
		i = connInfos.GetCount();
		while (i-- > 0)
		{
			connInfo = connInfos.GetItem(i);

			defGW = connInfo->GetDefaultGW();
			if (defGW != 0)
			{
				netMask = Net::SocketUtil::GetDefNetMaskv4(defGW);
				defIP = connInfo->GetIPAddress(0);
				j = 0;
				while (currIP = connInfo->GetIPAddress(j++))
				{
					if ((currIP & netMask) == (defGW & netMask))
					{
						defIP = currIP;
						break;
					}
				}
			}
			netMask = Net::SocketUtil::GetDefNetMaskv4(ip);
			j = 0;
			while (currIP = connInfo->GetIPAddress(j++))
			{
				if ((currIP & netMask) == (ip & netMask))
				{
					foundIP = currIP;
					break;
				}
			}
			DEL_CLASS(connInfo);
		}
		if (foundIP)
			return foundIP;
		return defIP;
	}
	else
	{
		return 0;
	}
}*/

Bool Net::OSSocketFactory::AdapterSetHWAddr(Text::CString adapterName, const UInt8 *hwAddr)
{
	return false;
}

Bool Net::OSSocketFactory::AdapterEnable(Text::CString adapterName, Bool enable)
{
	Text::StringBuilderUTF8 sbCmd;
	Text::StringBuilderUTF8 sb;
	sbCmd.AppendC(UTF8STRC("netsh interface set interface \""));
	sbCmd.Append(adapterName);
	if (enable)
	{
		sbCmd.AppendC(UTF8STRC("\" ENABLED"));
	}
	else
	{
		sbCmd.AppendC(UTF8STRC("\" DISABLED"));
	}
	Int32 ret = Manage::Process::ExecuteProcess(sbCmd.ToCString(), sb);
	return ret == 0;
}

UOSInt Net::OSSocketFactory::GetBroadcastAddrs(Data::ArrayList<UInt32> *addrs)
{
	UInt32 currIP;
	UInt32 netMask;
	UOSInt cnt = 0;
	UOSInt i;
	UOSInt j;
	Net::ConnectionInfo *connInfo;
	Data::ArrayList<Net::ConnectionInfo*> connInfos;
	if (this->GetConnInfoList(&connInfos))
	{
		i = connInfos.GetCount();
		while (i-- > 0)
		{
			connInfo = connInfos.GetItem(i);
			j = 0;
			while ((currIP = connInfo->GetIPAddress(j++)) != 0)
			{
				netMask = Net::SocketUtil::GetDefNetMaskv4(currIP);
				addrs->Add(currIP | (~netMask));
				cnt++;
			}
			DEL_CLASS(connInfo);
		}
		return cnt;
	}
	else
	{
		return 0;
	}
}


Net::SocketFactory::ErrorType Net::OSSocketFactory::FromSystemError(Int32 sysErr)
{
	switch (sysErr)
	{
	case WSAECONNABORTED:
		return Net::SocketFactory::ET_CONN_ABORT;
	case WSAECONNRESET:
		return Net::SocketFactory::ET_CONN_RESET;
	case WSAEDISCON:
		return Net::SocketFactory::ET_DISCONNECT;
	case WSAESHUTDOWN:
		return Net::SocketFactory::ET_SHUTDOWN;
	default:
		return Net::SocketFactory::ET_UNKNOWN;
	}
}
