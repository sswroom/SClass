#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/StreamReader.h"
#include "Manage/Process.h"
#include "Math/Math.h"
#include "Net/ConnectionInfo.h"
#include "Net/DNSClient.h"
#include "Net/DNSHandler.h"
#include "Net/OSSocketFactory.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"
#include "Text/TextBinEnc/Punycode.h"
#include <fcntl.h>
#include <unistd.h>
#include <netdb.h>
#if !defined(__ARM_PCS_VFP) && 0
#include <net/if.h>
#endif
#include <netinet/in.h>
#if defined(__APPLE__) || defined(__FreeBSD__)
#include <netinet/ip.h>
#endif
#include <netinet/ip_icmp.h>
#include <netinet/tcp.h>
#if !defined(__APPLE__) && !defined(__FreeBSD__)
#include <linux/if_ether.h>
#include <linux/if_arp.h>
#endif
#include <errno.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <wchar.h>

#if defined(__APPLE__) || defined(__FreeBSD__)
struct iphdr
{
	UInt8 ihl:4,
		version:4;
	UInt8 tos;
	UInt16 tot_len;
	UInt16 id;
	UInt16 frag_off;
	UInt8 ttl;
	UInt8 protocol;
	UInt16 check;
	UInt32 saddr;
	UInt32 daddr;
};
#endif
typedef struct
{
	int sock;
	const Char *name;
} ConnectionData;

Net::OSSocketFactory::OSSocketFactory(Bool noV6DNS) : Net::SocketFactory(noV6DNS)
{
/*	UnsafeArray<const UTF8Char> fileName = (const UTF8Char*)"/proc/sys/net/ipv6/bindv6only";
	if (IO::Path::GetPathType(fileName) == IO::Path::PathType::File)
	{
		IO::FileStream *fs;
		NEW_CLASS(fs, IO::FileStream(fileName, IO::FileMode::CreateWRITE, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		if (fs->IsError())
		{
			wprintf(L"bindv6only cannot open\r\n");
			DEL_CLASS(fs);
		}
		else
		{
			fs->Write((const UInt8*)"1", 1);
			DEL_CLASS(fs);
			wprintf(L"bindv6only set\r\n");
		}
	}
	else
	{
		wprintf(L"bindv6only not found\r\n");
	}*/
}

Net::OSSocketFactory::~OSSocketFactory()
{
	this->dnsHdlr.Delete();
}

Optional<Socket> Net::OSSocketFactory::CreateTCPSocketv4()
{
	int s = socket(AF_INET, SOCK_STREAM, 0) + 1;
	return (Socket*)(OSInt)s;
}

Optional<Socket> Net::OSSocketFactory::CreateTCPSocketv6()
{
	int s = socket(AF_INET6, SOCK_STREAM, 0) + 1;
	return (Socket*)(OSInt)s;
}

Optional<Socket> Net::OSSocketFactory::CreateUDPSocketv4()
{
	int s = socket(AF_INET, SOCK_DGRAM, 0) + 1;
	return (Socket*)(OSInt)s;
}

Optional<Socket> Net::OSSocketFactory::CreateUDPSocketv6()
{
	int s = socket(AF_INET6, SOCK_DGRAM, 0) + 1;
	return (Socket*)(OSInt)s;
}

Optional<Socket> Net::OSSocketFactory::CreateICMPIPv4Socket(UInt32 ip)
{
	int s = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP) + 1;
	return (Socket*)(OSInt)s;
}

Optional<Socket> Net::OSSocketFactory::CreateUDPRAWv4Socket(UInt32 ip)
{
	int s = socket(AF_INET, SOCK_RAW, IPPROTO_UDP) + 1;
	return (Socket*)(OSInt)s;
}

Optional<Socket> Net::OSSocketFactory::CreateRAWIPv4Socket(UInt32 ip)
{
	int s = socket(AF_INET, SOCK_RAW, IPPROTO_IP) + 1;
	return (Socket*)(OSInt)s;
}

Optional<Socket> Net::OSSocketFactory::CreateARPSocket()
{
#if defined(__APPLE__) || defined(__FreeBSD__)
	return 0;
#else
	int s = socket(AF_INET, SOCK_PACKET, htons(ETH_P_ARP)) + 1;
	if (s)
	{
		struct sockaddr_ll s_ll;
		MemClear(&s_ll, sizeof(s_ll));
		s_ll.sll_family = AF_PACKET;
		s_ll.sll_protocol = htons(ETH_P_ARP); 
		s_ll.sll_ifindex = 0; // all ifaces 
		bind(s - 1, (struct sockaddr *)&s_ll, sizeof(s_ll));
	}
	return (Socket*)(OSInt)s;
#endif
}

Optional<Socket> Net::OSSocketFactory::CreateRAWSocket()
{
#if defined(__APPLE__) || defined(__FreeBSD__)
	return 0;
#else
	int s = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL)) + 1;
	return (Socket*)(OSInt)s;
#endif
}

void Net::OSSocketFactory::DestroySocket(NN<Socket> socket)
{
	int fd = (Int32)this->SocketGetFD(socket);
	shutdown(fd, SHUT_RDWR);
	close(fd);
}

Bool Net::OSSocketFactory::SocketBindv4(NN<Socket> socket, UInt32 ip, UInt16 port)
{
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ip;
	addr.sin_port = htons(port);
	return bind((Int32)this->SocketGetFD(socket), (sockaddr*)&addr, sizeof(addr)) != -1;
}

Bool Net::OSSocketFactory::SocketBind(NN<Socket> socket, Optional<const Net::SocketUtil::AddressInfo> addr, UInt16 port)
{
	NN<const Net::SocketUtil::AddressInfo> nnaddr;
	if (!addr.SetTo(nnaddr))
	{
		UInt8 addrBuff[28];
		*(Int16*)&addrBuff[0] = AF_INET6;
		WriteMInt16(&addrBuff[2], port);
		WriteMInt32(&addrBuff[4], 0);
		MemClear(&addrBuff[8], 20);
		return bind((Int32)this->SocketGetFD(socket), (sockaddr*)&addrBuff, 28) != -1;
	}
	else if (nnaddr->addrType == Net::AddrType::IPv6)
	{
		UInt8 addrBuff[28];
		*(Int16*)&addrBuff[0] = AF_INET6;
		WriteMInt16(&addrBuff[2], port);
		WriteMInt32(&addrBuff[4], 0);
		MemCopyNO(&addrBuff[8], nnaddr->addr, 20);
		return bind((Int32)this->SocketGetFD(socket), (sockaddr*)&addrBuff, 28) != -1;
	}
	else if (nnaddr->addrType == Net::AddrType::IPv4)
	{
		sockaddr_in saddr;
		saddr.sin_family = AF_INET;
		saddr.sin_addr.s_addr = *(in_addr_t*)nnaddr->addr;
		saddr.sin_port = htons(port);
		return bind((Int32)this->SocketGetFD(socket), (sockaddr*)&saddr, sizeof(saddr)) != -1;
	}
	else
	{
		return false;
	}
}

Bool Net::OSSocketFactory::SocketBindRAWIf(NN<Socket> socket, UOSInt ifIndex)
{
#if defined(__APPLE__) || defined(__FreeBSD__)
	return false;
#else
	struct sockaddr_ll sll;
	MemClear(&sll, sizeof(sll));
	sll.sll_family          = AF_PACKET;
	sll.sll_ifindex         = (int)ifIndex;
	sll.sll_protocol        = htons(ETH_P_ALL);
	Bool ret = bind((Int32)this->SocketGetFD(socket), (struct sockaddr *) &sll, sizeof(sll)) != -1;

	struct packet_mreq mr;
	MemClear(&mr, sizeof(mr));
	mr.mr_ifindex = (int)ifIndex;
	mr.mr_type    = PACKET_MR_PROMISC;
	return (setsockopt((Int32)this->SocketGetFD(socket), SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mr, sizeof(mr)) != -1) && ret;
#endif
}

Bool Net::OSSocketFactory::SocketListen(NN<Socket> socket)
{
	return listen((Int32)this->SocketGetFD(socket), SOMAXCONN) != -1;
}

Optional<Socket> Net::OSSocketFactory::SocketAccept(NN<Socket> socket)
{
	sockaddr_in saddr;
	socklen_t addrlen = sizeof(saddr);
	int s;
	s = accept((Int32)this->SocketGetFD(socket), (sockaddr*)&saddr, &addrlen) + 1;
	return (Socket*)(OSInt)s;
}

Int32 Net::OSSocketFactory::SocketGetLastError()
{
	return errno;
}

Bool Net::OSSocketFactory::GetRemoteAddr(NN<Socket> socket, NN<Net::SocketUtil::AddressInfo> addr, OptOut<UInt16> port)
{
	sockaddr_storage addrBuff;
	sockaddr *saddr = (sockaddr*)&addrBuff;
	socklen_t size = sizeof(addrBuff);
	if (getpeername((Int32)this->SocketGetFD(socket), saddr, &size) == 0)
	{
		if (saddr->sa_family == AF_INET)
		{
			addr->addrType = Net::AddrType::IPv4;
			*(in_addr_t*)addr->addr = ((sockaddr_in*)&addrBuff)->sin_addr.s_addr;
			MemClear(&addr->addr[4], 16);
			if (port.IsNotNull())
			{
				port.SetNoCheck(ntohs(((sockaddr_in*)&addrBuff)->sin_port));
			}
			return true;
		}
		else if (saddr->sa_family == AF_INET6)
		{
			addr->addrType = Net::AddrType::IPv6;
			MemCopyNO(addr->addr, &saddr->sa_data[6], 20);
			if (port.IsNotNull())
			{
				port.SetNoCheck(ntohs(((sockaddr_in6*)&addrBuff)->sin6_port));
			}
			return true;
		}
		else
		{
			addr->addrType = Net::AddrType::Unknown;
		}
		return false;
	}
	else
	{
		addr->addrType = Net::AddrType::Unknown;
		return false;
	}
}

Bool Net::OSSocketFactory::GetLocalAddr(NN<Socket> socket, NN<Net::SocketUtil::AddressInfo> addr, OptOut<UInt16> port)
{
	sockaddr_storage addrBuff;
	sockaddr *saddr = (sockaddr*)&addrBuff;
	socklen_t size = sizeof(addrBuff);
	if (getsockname((Int32)this->SocketGetFD(socket), saddr, &size) == 0)
	{
		if (saddr->sa_family == AF_INET)
		{
			addr->addrType = Net::AddrType::IPv4;
			*(in_addr_t*)addr->addr = ((sockaddr_in*)&addrBuff)->sin_addr.s_addr;
			if (port.IsNotNull())
			{
				port.SetNoCheck(ntohs(((sockaddr_in*)&addrBuff)->sin_port));
			}
			return true;
		}
		else if (saddr->sa_family == AF_INET6)
		{
			addr->addrType = Net::AddrType::IPv6;
			MemCopyNO(addr->addr, &saddr->sa_data[6], 20);
			if (port.IsNotNull())
			{
				port.SetNoCheck(ntohs(((sockaddr_in6*)&addrBuff)->sin6_port));
			}
			return true;
		}
		else
		{
			addr->addrType = Net::AddrType::Unknown;
		}
		return false;
	}
	else
	{
		addr->addrType = Net::AddrType::Unknown;
		return false;
	}
}

OSInt Net::OSSocketFactory::SocketGetFD(NN<Socket> socket)
{
	return -1 + (OSInt)socket.Ptr();
}

Bool Net::OSSocketFactory::SocketWait(NN<Socket> socket, Data::Duration dur)
{
	fd_set fds;
	Int32 s = (Int32)SocketGetFD(socket);
	FD_ZERO(&fds);
	FD_SET(s, &fds);
	struct timeval tv;
	tv.tv_sec = (time_t)dur.GetSeconds();
	tv.tv_usec = (suseconds_t)(dur.GetNS() / 1000);
	int rc = select((int)(s + 1), &fds, NULL, NULL, &tv);
	if (rc == -1)
		return false;
	return rc != 0;
}

void Net::OSSocketFactory::SetDontLinger(NN<Socket> socket, Bool val)
{
	linger ling;
	socklen_t leng = sizeof(ling);
	Int32 ret = getsockopt((Int32)this->SocketGetFD(socket), SOL_SOCKET, SO_LINGER, &ling, &leng);
	if (ret != 0)
	{
		ling.l_linger = 30;
	}
	if (val)
	{
		ling.l_onoff = 1;
	}
	else
	{
		ling.l_onoff = 0;
	}
	ret = setsockopt((Int32)this->SocketGetFD(socket), SOL_SOCKET, SO_LINGER, &ling, sizeof(ling));
	if (ret != 0)
	{
	}
}

void Net::OSSocketFactory::SetLinger(NN<Socket> socket, UInt32 ms)
{
	linger ling;
	ling.l_onoff = 1;
	ling.l_linger = (Int32)ms / 1000;
	setsockopt((Int32)this->SocketGetFD(socket), SOL_SOCKET, SO_LINGER, &ling, sizeof(ling));
}

void Net::OSSocketFactory::SetRecvBuffSize(NN<Socket> socket, Int32 buffSize)
{
	setsockopt((Int32)this->SocketGetFD(socket), SOL_SOCKET, SO_RCVBUF, &buffSize, sizeof(Int32));
}

void Net::OSSocketFactory::SetNoDelay(NN<Socket> socket, Bool val)
{
	int v = val?1:0;
	setsockopt((Int32)this->SocketGetFD(socket), IPPROTO_TCP, TCP_NODELAY, (char*)&v, sizeof(v));
}

void Net::OSSocketFactory::SetSendTimeout(NN<Socket> socket, Data::Duration timeout)
{
	struct timeval tv;
#if defined(__APPLE__) || defined(__DEFINED_time_t)
	tv.tv_sec = (time_t)timeout.GetSeconds();
	tv.tv_usec = (suseconds_t)timeout.GetNS() / 1000;
#else
	tv.tv_sec = (__time_t)timeout.GetSeconds();
	tv.tv_usec = (__suseconds_t)timeout.GetNS() / 1000;
#endif
	if (setsockopt((Int32)this->SocketGetFD(socket), SOL_SOCKET, SO_SNDTIMEO, (char*)&tv, sizeof(tv)))
	{
		printf("Error in set SO_SNDTIMEO\r\n");
	}
}

void Net::OSSocketFactory::SetRecvTimeout(NN<Socket> socket, Data::Duration timeout)
{
	struct timeval tv;
#if defined(__APPLE__) || defined(__DEFINED_time_t)
	tv.tv_sec = (time_t)timeout.GetSeconds();
	tv.tv_usec = (suseconds_t)timeout.GetNS() / 1000;
#else
	tv.tv_sec = (__time_t)timeout.GetSeconds();
	tv.tv_usec = (__suseconds_t)timeout.GetNS() / 1000;
#endif
	if (setsockopt((Int32)this->SocketGetFD(socket), SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(tv)))
	{
		printf("Error in set SO_RCVTIMEO\r\n");
	}
}

void Net::OSSocketFactory::SetReuseAddr(NN<Socket> socket, Bool val)
{
	int v = val?1:0;
	setsockopt((Int32)this->SocketGetFD(socket), SOL_SOCKET, SO_REUSEADDR, (char*)&v, sizeof(v));
}

void Net::OSSocketFactory::SetIPv4TTL(NN<Socket> socket, Int32 ttl)
{
	setsockopt((Int32)this->SocketGetFD(socket), IPPROTO_IP, IP_TTL, (char*)&ttl, sizeof(ttl));
}

void Net::OSSocketFactory::SetBroadcast(NN<Socket> socket, Bool val)
{
	int v = val?1:0;
	setsockopt((Int32)this->SocketGetFD(socket), SOL_SOCKET, SO_BROADCAST, (char*)&v, sizeof(v));
}

void Net::OSSocketFactory::AddIPMembership(NN<Socket> socket, UInt32 ip)
{
	struct ip_mreq mreq;
	mreq.imr_multiaddr.s_addr = ip;
	mreq.imr_interface.s_addr = 0;
	setsockopt((Int32)this->SocketGetFD(socket), IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq));
}

UOSInt Net::OSSocketFactory::SendData(NN<Socket> socket, UnsafeArray<const UInt8> buff, UOSInt buffSize, OptOut<ErrorType> et)
{
	int flags = 0;
#if defined(MSG_NOSIGNAL)
	flags = MSG_NOSIGNAL;
#endif
	OSInt ret = send((Int32)this->SocketGetFD(socket), (const char*)buff.Ptr(), (size_t)buffSize, flags);
	if (ret == -1)
	{
		if (et.IsNotNull())
		{
			switch (errno)
			{
			case ECONNRESET:
				et.SetNoCheck(ET_CONN_RESET);
				break;
			case ECONNABORTED:
				et.SetNoCheck(ET_CONN_ABORT);
				break;
			case ENOTCONN:
				et.SetNoCheck(ET_DISCONNECT);
				break;
			case ENOBUFS:
				et.SetNoCheck(ET_SHUTDOWN);
				break;
			default:
				et.SetNoCheck(ET_UNKNOWN);
				break;
			}
		}
		return 0;
	}
	else
	{
		et.Set(ET_NO_ERROR);
		return (UOSInt)ret;
	}
}

UOSInt Net::OSSocketFactory::ReceiveData(NN<Socket> socket, UnsafeArray<UInt8> buff, UOSInt buffSize, OptOut<ErrorType> et)
{
	OSInt ret = recv((Int32)this->SocketGetFD(socket), (char*)buff.Ptr(), (size_t)buffSize, 0);
//	OSInt ret = read(this->SocketGetFD(socket), (char*)buff, (int)buffSize);
	if (ret == -1)
	{
		if (et.IsNotNull())
		{
			switch (errno)
			{
			case ECONNRESET:
				et.SetNoCheck(ET_CONN_RESET);
				break;
			case ECONNABORTED:
				et.SetNoCheck(ET_CONN_ABORT);
				break;
			case ENOTCONN:
				et.SetNoCheck(ET_DISCONNECT);
				break;
			case ENOBUFS:
				et.SetNoCheck(ET_SHUTDOWN);
				break;
			default:
				et.SetNoCheck(ET_UNKNOWN);
				break;
			}
		}
		return 0;
	}
	else
	{
		et.Set(ET_NO_ERROR);
		return (UOSInt)ret;
	}
}

Optional<Net::SocketRecvSess> Net::OSSocketFactory::BeginReceiveData(NN<Socket> socket, UnsafeArray<UInt8> buff, UOSInt buffSize, NN<Sync::Event> evt, OptOut<ErrorType> et)
{
	UOSInt ret = ReceiveData(socket, buff, buffSize, et);
	if (ret)
	{
		evt->Set();
	}
	return (SocketRecvSess*)ret;
}

UOSInt Net::OSSocketFactory::EndReceiveData(NN<Net::SocketRecvSess> reqData, Bool toWait, OutParam<Bool> incomplete)
{
	incomplete.Set(false);
	return (UOSInt)reqData.Ptr();
}

void Net::OSSocketFactory::CancelReceiveData(NN<Net::SocketRecvSess> reqData)
{
}

UOSInt Net::OSSocketFactory::UDPReceive(NN<Socket> socket, UnsafeArray<UInt8> buff, UOSInt buffSize, NN<Net::SocketUtil::AddressInfo> addr, OutParam<UInt16> port, OptOut<ErrorType> et)
{
	OSInt recvSize;
	sockaddr_storage addrBuff;
	sockaddr *saddr = (sockaddr*)&addrBuff;
	socklen_t size = sizeof(addrBuff);
	saddr->sa_family = 0;
	recvSize = recvfrom((Int32)this->SocketGetFD(socket), (Char*)buff.Ptr(), (size_t)buffSize, 0, saddr, (socklen_t*)&size);
	if (recvSize <= 0)
	{
//		printf("UDP recv error: %d\r\n", errno);
		if (et.IsNotNull())
		{
			if (errno == ECONNRESET)
			{
				et.SetNoCheck(ET_CONN_RESET);
			}
			else if (errno == ECONNRESET)
			{
				et.SetNoCheck(ET_CONN_ABORT);
			}
			else if (errno == ENOTCONN)
			{
				et.SetNoCheck(ET_DISCONNECT);
			}
			else if (errno == ENOBUFS)
			{
				et.SetNoCheck(ET_SHUTDOWN);
			}
			else
			{
				et.SetNoCheck(ET_UNKNOWN);
			}
		}
		return 0;
	}
	else
	{
		if (saddr->sa_family == AF_INET)
		{
			addr->addrType = Net::AddrType::IPv4;
			*(in_addr_t*)addr->addr = ((sockaddr_in*)&addrBuff)->sin_addr.s_addr;
			port.Set(ntohs(((sockaddr_in*)&addrBuff)->sin_port));
			return (UOSInt)recvSize;
		}
		else if (saddr->sa_family == AF_INET6)
		{
			addr->addrType = Net::AddrType::IPv6;
			MemCopyNO(addr->addr, &saddr->sa_data[6], 20);
			port.Set(ntohs(((sockaddr_in6*)&addrBuff)->sin6_port));
			return (UOSInt)recvSize;
		}
		else
		{
//			printf("UDPReceive: unknown family %d\r\n", saddr->sa_family);
		}
		
		return (UOSInt)recvSize;
	}
}

UOSInt Net::OSSocketFactory::SendTo(NN<Socket> socket, UnsafeArray<const UInt8> buff, UOSInt buffSize, NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port)
{
	sockaddr_storage addrBase;
	sockaddr *addrBuff = (sockaddr*)&addrBase;
	socklen_t addrSize;
/*	if (addr == 0)
	{
		MemClear(addrBuff, sizeof(sockaddr));
		addrBuff->sa_family = AF_INET;
		addrSize = sizeof(sockaddr);
	}*/
	if (addr->addrType == Net::AddrType::IPv4)
	{
		sockaddr_in *saddr = (sockaddr_in*)&addrBase;
		saddr->sin_family = AF_INET;
		saddr->sin_addr.s_addr = *(in_addr_t*)addr->addr;
		saddr->sin_port = htons(port);
		addrSize = sizeof(sockaddr_in);
	}
	else if (addr->addrType == Net::AddrType::IPv6)
	{
		UInt8 *saddr = (UInt8*)addrBuff;
		*(Int16*)&saddr[0] = AF_INET6;
		WriteMInt16(&saddr[2], port);
		WriteMInt32(&saddr[4], 0);
		MemCopyNO(&saddr[8], addr->addr, 20);
		addrSize = 28;
	}
	else
	{
		return 0;
	}
	OSInt ret = sendto((Int32)this->SocketGetFD(socket), (const char*)buff.Ptr(), (size_t)buffSize, 0, addrBuff, addrSize);
	if (ret == -1)
	{
		return 0;
	}
	else
	{
		return (UOSInt)ret;
	}
}

UOSInt Net::OSSocketFactory::SendToIF(NN<Socket> socket, UnsafeArray<const UInt8> buff, UOSInt buffSize, UnsafeArray<const UTF8Char> ifName)
{
	sockaddr addrBase;
	sockaddr *addrBuff = (sockaddr*)&addrBase;
	socklen_t addrSize;
	MemClear(addrBuff, sizeof(sockaddr));
	addrBuff->sa_family = AF_INET;
	Text::StrConcat(addrBuff->sa_data, (const Char*)ifName.Ptr());
	addrSize = sizeof(sockaddr);

	OSInt ret = sendto((Int32)this->SocketGetFD(socket), (const char*)buff.Ptr(), (size_t)buffSize, 0, addrBuff, addrSize);
	if (ret == -1)
	{
		return 0;
	}
	else
	{
		return (UOSInt)ret;
	}
}

UInt16 ICMPChecksum(UInt8 *buff, OSInt buffSize)
{
    UInt32 sum = 0xffff;
    while (buffSize > 1)
	{
        sum += ReadUInt16(buff);
        buff += 2;
        buffSize -= 2;
    }

    if(buffSize == 1)
        sum += buff[0];

    sum = (sum & 0xffff) + (sum >> 16);
    sum = (sum & 0xffff) + (sum >> 16);

    return (UInt16)~sum;
}

Bool Net::OSSocketFactory::IcmpSendEcho2(NN<const Net::SocketUtil::AddressInfo> addr, OutParam<UInt32> respTime_us, OutParam<UInt32> ttl)
{
	int rs;
	if (addr->addrType == Net::AddrType::IPv4)
	{
		rs = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP);
	}
	else if (addr->addrType == Net::AddrType::IPv6)
	{
		rs = socket(PF_INET6, SOCK_RAW, IPPROTO_ICMP);
	}
	else
	{
		return false;
	}
	if (rs >= 0)
	{
		sockaddr_storage addrBase;
		sockaddr *addrBuff = (sockaddr*)&addrBase;
		socklen_t addrSize;
		if (addr->addrType == Net::AddrType::IPv4)
		{
			sockaddr_in *saddr = (sockaddr_in*)&addrBase;
			saddr->sin_family = AF_INET;
			saddr->sin_addr.s_addr = *(in_addr_t*)addr->addr;
			saddr->sin_port = 0;
			addrSize = sizeof(sockaddr_in);
		}
		else if (addr->addrType == Net::AddrType::IPv6)
		{
			UInt8 *saddr = (UInt8*)addrBuff;
			*(Int16*)&saddr[0] = AF_INET6;
			WriteMInt16(&saddr[2], 0);
			WriteMInt32(&saddr[4], 0);
			MemCopyNO(&saddr[8], addr->addr, 20);
			addrSize = 28;
		}
		else
		{
			close(rs);
			return false;
		}

		UInt8 hdr[56];
		MemClear(hdr, sizeof(hdr));
		hdr[0] = ICMP_ECHO; //type
		hdr[1] = 0; //code
		WriteInt16(&hdr[2], 0); //checksum
		WriteInt16(&hdr[4], 0); //id
		WriteInt16(&hdr[6], 0); //seq;
		WriteInt16(&hdr[2], ICMPChecksum(hdr, sizeof(hdr)));
		OSInt retSize = sendto(rs, (const char*)hdr, sizeof(hdr), 0, addrBuff, addrSize);
		Int64 timeStart = Manage::HiResClock::GetRelTime_us();
		if (retSize < 1)
		{
			close(rs);
			return false;
		}
		UInt8 buff[1024];
		retSize = recv(rs, buff, 1024, 0);
		if (retSize < 1)
		{
			close(rs);
			return false;
		}
		timeStart = Manage::HiResClock::GetRelTime_us() - timeStart;

		Bool succ = false;
		UInt8 *icmphdrptr;
		struct iphdr *iphdrptr;
		iphdrptr = (struct iphdr*)buff;
		icmphdrptr = (UInt8*)(buff + (iphdrptr->ihl) * 4);
		switch (icmphdrptr[0]) //type
		{
		case 8:
		case 0:
			ttl.Set(iphdrptr->ttl);
			respTime_us.Set((UInt32)(UInt64)timeStart);
			succ = true;
			break;
		case 3:
		default:
			break;
		}

		close(rs);
		return succ;
	}
	else
	{
		UTF8Char sbuff[64];
		UnsafeArray<UTF8Char> sptr;
		if (addr->addrType == Net::AddrType::IPv6)
		{
			sptr = Text::StrConcatC(sbuff, UTF8STRC("ping6 -c 1 "));
		}
		else
		{
			sptr = Text::StrConcatC(sbuff, UTF8STRC("ping -c 1 "));
		}
		sptr = Net::SocketUtil::GetAddrName(sptr, addr).Or(sptr);
		Text::StringBuilderUTF8 sb;
		Int32 ret;
		ret = Manage::Process::ExecuteProcess(CSTRP(sbuff, sptr), sb);
		if (ret == 0)
		{
			Text::PString sarr[4];
			UOSInt i = Text::StrSplitLineP(sarr, 3, sb);
			if (i == 3)
			{
				i = Text::StrIndexOfC(sarr[1].v, sarr[1].leng, UTF8STRC(": "));
				UnsafeArray<UTF8Char> linePtr = sarr[1].v;
				UOSInt lineLen = sarr[1].leng;
				if (i != INVALID_INDEX)
				{
					linePtr = &sarr[1].v[i + 2];
					lineLen = sarr[1].leng - i - 2;
				}
				if (Text::StrStartsWithC(linePtr, lineLen, UTF8STRC("icmp_seq=")))
				{
					if (Text::StrSplitWSP(sarr, 4, {linePtr, lineLen}) == 4)
					{
						ttl.Set(Text::StrToUInt32(&sarr[1].v[4]));
						respTime_us.Set((UInt32)Double2Int32(Text::StrToDoubleOr(&sarr[2].v[5], 0) * 1000.0));
						return true;
					}
					else
					{
						return false;
					}
				}
				else
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
	}
	return false;
}

Bool Net::OSSocketFactory::Connect(NN<Socket> socket, UInt32 ip, UInt16 port, Data::Duration timeout)
{
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ip;
	addr.sin_port = htons(port);

/*	fd_set fdset;
	struct timeval tv;

	Bool succ = false;
	long arg = fcntl(this->SocketGetFD(socket), F_GETFL, 0);
    fcntl(this->SocketGetFD(socket), F_SETFL, arg | O_NONBLOCK);

    connect(this->SocketGetFD(socket), (struct sockaddr *)&addr, sizeof(addr));

    FD_ZERO(&fdset);
    FD_SET(this->SocketGetFD(socket), &fdset);
    tv.tv_sec = 10;
    tv.tv_usec = 0;

    if (select(this->SocketGetFD(socket), NULL, &fdset, NULL, &tv) == 1) //sock + 1
    {
        int so_error;
        socklen_t len = sizeof so_error;

        getsockopt(this->SocketGetFD(socket), SOL_SOCKET, SO_ERROR, &so_error, &len);

        if (so_error == 0)
		{
			succ = true;
        }
    }

    fcntl(this->SocketGetFD(socket), F_SETFL, arg);

	return succ;*/
	if (timeout.NotZero())
		this->SetSendTimeout(socket, timeout);
    return connect((Int32)this->SocketGetFD(socket), (struct sockaddr *)&addr, sizeof(addr)) == 0;
}

Bool Net::OSSocketFactory::Connect(NN<Socket> socket, NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::Duration timeout)
{
	UInt8 addrBuff[28];
	socklen_t socklen;
	if (addr->addrType == Net::AddrType::IPv4)
	{
		*(Int16*)&addrBuff[0] = AF_INET;
		WriteMInt16(&addrBuff[2], port);
		*(Int32*)&addrBuff[4] = *(Int32*)addr->addr;
		socklen = sizeof(sockaddr_in);
	}
	else if (addr->addrType == Net::AddrType::IPv6)
	{
		*(Int16*)&addrBuff[0] = AF_INET6;
		WriteMInt16(&addrBuff[2], port);
		WriteMInt32(&addrBuff[4], 0);
		MemCopyNO(&addrBuff[8], addr->addr, 20);
		socklen = 28;
	}
	else
	{
		return false;
	}
	if (timeout.NotZero())
		this->SetSendTimeout(socket, timeout);
	return connect((Int32)this->SocketGetFD(socket), (const sockaddr*)addrBuff, socklen) == 0;
}

void Net::OSSocketFactory::ShutdownSend(NN<Socket> socket)
{
	shutdown((Int32)this->SocketGetFD(socket), SHUT_WR);
}

void Net::OSSocketFactory::ShutdownSocket(NN<Socket> socket)
{
	shutdown((Int32)this->SocketGetFD(socket), SHUT_RDWR);
}

Bool Net::OSSocketFactory::SocketGetReadBuff(NN<Socket> socket, OutParam<UInt32> size)
{
/*	struct timeval tv;
	fd_set readset;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	FD_ZERO(&readset);
	FD_SET(this->SocketGetFD(socket), &readset);
	int result = select(1, &readset, 0, 0, &tv);
	Bool ret = false;
	if (result <= 0)
	{
		*size = 0;
	}
	else if (FD_ISSET(this->SocketGetFD(socket), &readset))
	{
		ret = true;
		*size = 1;
	}*/

	return ioctl((Int32)this->SocketGetFD(socket), FIONREAD, (int*)size.Ptr()) == 0;
}

Bool Net::OSSocketFactory::DNSResolveIPDef(UnsafeArray<const Char> host, NN<Net::SocketUtil::AddressInfo> addr)
{
	addrinfo *result = 0;
	Int32 iResult = getaddrinfo(host.Ptr(), 0, 0, &result);
	if (iResult == 0)
	{
		Bool succ = false;
		if (result->ai_addr->sa_family == AF_INET)
		{
			addr->addrType = Net::AddrType::IPv4;
			*(in_addr_t*)addr->addr = ((sockaddr_in*)result->ai_addr)->sin_addr.s_addr;
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

Bool Net::OSSocketFactory::GetDefDNS(NN<Net::SocketUtil::AddressInfo> addr)
{
	IO::FileStream fs(CSTR("/etc/resolv.conf"), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (fs.IsError())
	{
		return false;
	}
	Bool ret = false;
	Text::PString sarr[3];
	Text::StringBuilderUTF8 sb;
	Text::UTF8Reader reader(fs);
	while (true)
	{
		sb.ClearStr();
		if (!reader.ReadLine(sb, 1024))
		{
			break;
		}
		sb.Trim();
		if (sb.StartsWith(UTF8STRC("#")))
		{
		}
		else
		{
			if (Text::StrSplitTrimP(sarr, 3, sb, ' ') == 2)
			{
				if (sarr[0].Equals(UTF8STRC("nameserver")))
				{
					if (Net::SocketUtil::SetAddrInfo(addr, sarr[1].ToCString()))
					{
						ret = true;
						break;
					}
				}
			}
		}
	}
	return ret;
}

UOSInt Net::OSSocketFactory::GetDNSList(NN<Data::ArrayList<UInt32>> dnsList)
{
	IO::FileStream fs(CSTR("/etc/resolv.conf"), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (fs.IsError())
	{
		return false;
	}
	UOSInt ret = 0;
	Text::PString sarr[3];
	Text::StringBuilderUTF8 sb;
	Net::SocketUtil::AddressInfo addr;
	Text::UTF8Reader reader(fs);
	while (true)
	{
		sb.ClearStr();
		if (!reader.ReadLine(sb, 1024))
		{
			break;
		}
		sb.Trim();
		if (sb.StartsWith(UTF8STRC("#")))
		{
		}
		else
		{
			if (Text::StrSplitTrimP(sarr, 3, sb, ' ') == 2)
			{
				if (sarr[0].Equals(UTF8STRC("nameserver")))
				{
					if (Net::SocketUtil::SetAddrInfo(addr, sarr[1].ToCString()))
					{
						if (addr.addrType == Net::AddrType::IPv4)
						{
							ret++;
							dnsList->Add(*(UInt32*)addr.addr);
						}
					}
				}
			}
		}
	}
	return ret;
}

Bool Net::OSSocketFactory::LoadHosts(NN<Net::DNSHandler> dnsHdlr)
{
	Net::SocketUtil::AddressInfo addr;
	UOSInt i;
	Text::PString sarr[2];
	IO::FileStream fs(CSTR("/etc/hosts"), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
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
						dnsHdlr->AddHost(addr, sarr[0].ToCString());
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

Bool Net::OSSocketFactory::ARPAddRecord(UOSInt ifIndex, UnsafeArray<const UInt8> hwAddr, UInt32 ipv4)
{
	return false;
}


UOSInt Net::OSSocketFactory::GetConnInfoList(NN<Data::ArrayListNN<Net::ConnectionInfo>> connInfoList)
{
	int sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
	if (sock == -1)
		return 0;

	NN<Net::ConnectionInfo> connInfo;
	UOSInt ret = 0;
#if !defined(__APPLE__)
	ConnectionData data;
	Char buff[1024];
	ifconf ifc;
	ifreq *ifrend;
	ifreq *ifrcurr;

	ifc.ifc_len = sizeof(buff);
	ifc.ifc_buf = buff;
	if (ioctl(sock, SIOCGIFCONF, &ifc) >= 0)
	{
		ifrcurr = ifc.ifc_req;
		ifrend = ifrcurr + ((UOSInt)(UInt32)ifc.ifc_len / sizeof(ifreq));
		while (ifrcurr != ifrend)
		{
			data.sock = sock;
			data.name = ifrcurr->ifr_name;
			NEW_CLASSNN(connInfo, Net::ConnectionInfo(&data));
			connInfoList->Add(connInfo);
			ret++;
			ifrcurr++;
		}
	}
#endif
	close(sock);

	UTF8Char sbuff[128];
	Text::StringBuilderUTF8 sb;
	UInt32 ip;
	UInt32 gw;
	UOSInt i;
	UnsafeArray<UTF8Char> sarr[4];
	IO::FileStream fs(CSTR("/proc/net/route"), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (!fs.IsError())
	{
		Text::UTF8Reader reader(fs);
		if (reader.ReadLine(sb, 512))
		{
			sb.ClearStr();
			while (reader.ReadLine(sb, 512))
			{
				i = Text::StrSplitWS(sarr, 4, sb.v);
				if (i == 4)
				{
					ip = Text::StrHex2UInt32C(sarr[1]);
					gw = Text::StrHex2UInt32C(sarr[2]);
					if (ip == 0 && gw != 0)
					{
						i = connInfoList->GetCount();
						while (i-- > 0)
						{
							connInfo = connInfoList->GetItemNoCheck(i);
							connInfo->GetName(sbuff);
							if (Text::StrEquals(sbuff, sarr[0]))
							{
								connInfo->SetInfo(&gw);
								break;
							}
						}
					}
				}
				sb.ClearStr();
			}
		}
	}
	return ret;
}

Bool Net::OSSocketFactory::GetIPInfo(NN<IPInfo> info)
{
	info->dwForwarding = 0;
	info->dwDefaultTTL = 0;
	info->dwInReceives = 0;
	info->dwInHdrErrors = 0;
	info->dwInAddrErrors = 0;
	info->dwForwDatagrams = 0;
	info->dwInUnknownProtos = 0;
	info->dwInDiscards = 0;
	info->dwInDelivers = 0;
	info->dwOutRequests = 0;
	info->dwRoutingDiscards = 0;
	info->dwOutDiscards = 0;
	info->dwOutNoRoutes = 0;
	info->dwReasmTimeout = 0;
	info->dwReasmReqds = 0;
	info->dwReasmOks = 0;
	info->dwReasmFails = 0;
	info->dwFragOks = 0;
	info->dwFragFails = 0;
	info->dwFragCreates = 0;
	info->dwNumIf = 0;
	info->dwNumAddr = 0;
	info->dwNumRoutes = 0;
	return true;
}

Bool Net::OSSocketFactory::GetTCPInfo(NN<TCPInfo> info)
{
	info->dwRtoAlgorithm = 0;
	info->dwRtoMin = 0;
	info->dwRtoMax = 0;
	info->dwMaxConn = 0;
	info->dwActiveOpens = 0;
	info->dwPassiveOpens = 0;
	info->dwAttemptFails = 0;
	info->dwEstabResets = 0;
	info->dwCurrEstab = 0;
	info->dwInSegs = 0;
	info->dwOutSegs = 0;
	info->dwRetransSegs = 0;
	info->dwInErrs = 0;
	info->dwOutRsts = 0;
	info->dwNumConns = 0;
	return true;
}

Bool Net::OSSocketFactory::GetUDPInfo(NN<UDPInfo> info)
{
	info->dwInDatagrams = 0;
	info->dwNoPorts = 0;
	info->dwInErrors = 0;
	info->dwOutDatagrams = 0;
	info->dwNumAddrs = 0;
	return true;
}

UOSInt OSSocketFactory_LoadPortInfo(NN<Data::ArrayListNN<Net::SocketFactory::PortInfo>> portInfoList, Text::CStringNN path, Net::SocketFactory::ProtocolType protoType)
{
	UOSInt ret = 0;
	NN<Net::SocketFactory::PortInfo> port;
	IO::FileStream fs(path, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (fs.IsError())
	{
		
	}
	else
	{
		UnsafeArray<UTF8Char> sarr[5];
		UnsafeArray<UTF8Char> sarr2[3];
		Text::StringBuilderUTF8 sb;
		Text::UTF8Reader reader(fs);
		if (reader.ReadLine(sb, 1024))
		{
			while (true)
			{
				sb.ClearStr();
				if (!reader.ReadLine(sb, 1024))
				{
					break;
				}
				sb.Trim();
				if (Text::StrSplitWS(sarr, 5, sb.v) == 5)
				{
					port = MemAllocNN(Net::SocketFactory::PortInfo);
					port->protoType = protoType;
					if (Text::StrSplit(sarr2, 3, sarr[1], ':') == 2)
					{
						port->localIP = (UInt32)Text::StrHex2Int32C(sarr2[0]);
						port->localPort = (UInt16)Text::StrHex2Int16C(sarr2[1]);	
					}
					else
					{
						port->localIP = 0;
						port->localPort = 0;
					}
					if (Text::StrSplit(sarr2, 3, sarr[2], ':') == 2)
					{
						port->foreignIP = (UInt32)Text::StrHex2Int32C(sarr2[0]);
						port->foreignPort = (UInt16)Text::StrHex2Int16C(sarr2[1]);	
					}
					else
					{
						port->foreignIP = 0;
						port->foreignPort = 0;
					}
					switch (Text::StrHex2Int32C(sarr[3]))
					{
					case 1:
						port->portState = Net::SocketFactory::PS_ESTAB;
						break;
					case 2:
						port->portState = Net::SocketFactory::PS_SYN_SENT;
						break;
					case 3:
						port->portState = Net::SocketFactory::PS_SYN_RCVD;
						break;
					case 4:
						port->portState = Net::SocketFactory::PS_FIN_WAIT1;
						break;
					case 5:
						port->portState = Net::SocketFactory::PS_FIN_WAIT2;
						break;
					case 6:
						port->portState = Net::SocketFactory::PS_TIME_WAIT;
						break;
					case 7:
						port->portState = Net::SocketFactory::PS_CLOSED;
						break;
					case 8:
						port->portState = Net::SocketFactory::PS_CLOSE_WAIT;
						break;
					case 9:
						port->portState = Net::SocketFactory::PS_LAST_ACK;
						break;
					case 10:
						port->portState = Net::SocketFactory::PS_LISTEN;
						break;
					case 11:
						port->portState = Net::SocketFactory::PS_CLOSING;
						break;
					default:
						port->portState = Net::SocketFactory::PS_UNKNOWN;
						break;
					}
					port->processId = 0;
					portInfoList->Add(port);
					ret++;
				}
			}
		}
	}
	return ret;
}

UOSInt OSSocketFactory_LoadPortInfov4(NN<Data::ArrayListNN<Net::SocketFactory::PortInfo3>> portInfoList, Text::CStringNN path, Net::SocketFactory::ProtocolType protoType)
{
	UOSInt ret = 0;
	NN<Net::SocketFactory::PortInfo3> port;
	IO::FileStream fs(path, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (fs.IsError())
	{
		
	}
	else
	{
		UnsafeArray<UTF8Char> sarr[11];
		UnsafeArray<UTF8Char> sarr2[3];
		Text::StringBuilderUTF8 sb;
		Text::UTF8Reader reader(fs);
		if (reader.ReadLine(sb, 1024))
		{
			while (true)
			{
				sb.ClearStr();
				if (!reader.ReadLine(sb, 1024))
				{
					break;
				}
				sb.Trim();
				if (Text::StrSplitWS(sarr, 11, sb.v) == 11)
				{
					port = MemAllocNN(Net::SocketFactory::PortInfo3);
					port->protoType = protoType;
					if (Text::StrSplit(sarr2, 3, sarr[1], ':') == 2)
					{
						Net::SocketUtil::SetAddrInfoV4(port->localAddr, (UInt32)Text::StrHex2Int32C(sarr2[0]));
						port->localPort = (UInt16)Text::StrHex2Int16C(sarr2[1]);	
					}
					else
					{
						Net::SocketUtil::SetAddrInfoV4(port->localAddr, 0);
						port->localPort = 0;
					}
					if (Text::StrSplit(sarr2, 3, sarr[2], ':') == 2)
					{
						Net::SocketUtil::SetAddrInfoV4(port->foreignAddr, (UInt32)Text::StrHex2Int32C(sarr2[0]));
						port->foreignPort = (UInt16)Text::StrHex2Int16C(sarr2[1]);	
					}
					else
					{
						Net::SocketUtil::SetAddrInfoV4(port->foreignAddr, 0);
						port->foreignPort = 0;
					}
					switch (Text::StrHex2Int32C(sarr[3]))
					{
					case 1:
						port->portState = Net::SocketFactory::PS_ESTAB;
						break;
					case 2:
						port->portState = Net::SocketFactory::PS_SYN_SENT;
						break;
					case 3:
						port->portState = Net::SocketFactory::PS_SYN_RCVD;
						break;
					case 4:
						port->portState = Net::SocketFactory::PS_FIN_WAIT1;
						break;
					case 5:
						port->portState = Net::SocketFactory::PS_FIN_WAIT2;
						break;
					case 6:
						port->portState = Net::SocketFactory::PS_TIME_WAIT;
						break;
					case 7:
						port->portState = Net::SocketFactory::PS_CLOSED;
						break;
					case 8:
						port->portState = Net::SocketFactory::PS_CLOSE_WAIT;
						break;
					case 9:
						port->portState = Net::SocketFactory::PS_LAST_ACK;
						break;
					case 10:
						port->portState = Net::SocketFactory::PS_LISTEN;
						break;
					case 11:
						port->portState = Net::SocketFactory::PS_CLOSING;
						break;
					default:
						port->portState = Net::SocketFactory::PS_UNKNOWN;
						break;
					}
					port->processId = 0;
					port->socketId = Text::StrToUInt32(sarr[9]);
					portInfoList->Add(port);
					ret++;
				}
			}
		}
	}
	return ret;
}

UOSInt OSSocketFactory_LoadPortInfov6(NN<Data::ArrayListNN<Net::SocketFactory::PortInfo3>> portInfoList, Text::CStringNN path, Net::SocketFactory::ProtocolType protoType)
{
	UOSInt ret = 0;
	NN<Net::SocketFactory::PortInfo3> port;
	IO::FileStream fs(path, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (fs.IsError())
	{
		
	}
	else
	{
		UInt8 addr[32];
		UnsafeArray<UTF8Char> sarr[11];
		UnsafeArray<UTF8Char> sarr2[3];
		Text::StringBuilderUTF8 sb;
		Text::UTF8Reader reader(fs);
		if (reader.ReadLine(sb, 1024))
		{
			while (true)
			{
				sb.ClearStr();
				if (!reader.ReadLine(sb, 1024))
				{
					break;
				}
				sb.Trim();
				if (Text::StrSplitWS(sarr, 11, sb.v) == 11)
				{
					port = MemAllocNN(Net::SocketFactory::PortInfo3);
					port->protoType = protoType;
					if (Text::StrSplit(sarr2, 3, sarr[1], ':') == 2)
					{
						Text::StrHex2Bytes(sarr2[0], addr);
						WriteUInt32(&addr[0], ReadMUInt32(&addr[0]));
						WriteUInt32(&addr[4], ReadMUInt32(&addr[4]));
						WriteUInt32(&addr[8], ReadMUInt32(&addr[8]));
						WriteUInt32(&addr[12], ReadMUInt32(&addr[12]));
						Net::SocketUtil::SetAddrInfoV6(port->localAddr, addr, 0);
						port->localPort = (UInt16)Text::StrHex2Int16C(sarr2[1]);	
					}
					else
					{
						MemClear(addr, 16);
						Net::SocketUtil::SetAddrInfoV6(port->localAddr, addr, 0);
						port->localPort = 0;
					}
					if (Text::StrSplit(sarr2, 3, sarr[2], ':') == 2)
					{
						Text::StrHex2Bytes(sarr2[0], addr);
						WriteUInt32(&addr[0], ReadMUInt32(&addr[0]));
						WriteUInt32(&addr[4], ReadMUInt32(&addr[4]));
						WriteUInt32(&addr[8], ReadMUInt32(&addr[8]));
						WriteUInt32(&addr[12], ReadMUInt32(&addr[12]));
						Net::SocketUtil::SetAddrInfoV6(port->foreignAddr, addr, 0);
						port->foreignPort = (UInt16)Text::StrHex2Int16C(sarr2[1]);	
					}
					else
					{
						MemClear(addr, 16);
						Net::SocketUtil::SetAddrInfoV6(port->foreignAddr, addr, 0);
						port->foreignPort = 0;
					}
					switch (Text::StrHex2Int32C(sarr[3]))
					{
					case 1:
						port->portState = Net::SocketFactory::PS_ESTAB;
						break;
					case 2:
						port->portState = Net::SocketFactory::PS_SYN_SENT;
						break;
					case 3:
						port->portState = Net::SocketFactory::PS_SYN_RCVD;
						break;
					case 4:
						port->portState = Net::SocketFactory::PS_FIN_WAIT1;
						break;
					case 5:
						port->portState = Net::SocketFactory::PS_FIN_WAIT2;
						break;
					case 6:
						port->portState = Net::SocketFactory::PS_TIME_WAIT;
						break;
					case 7:
						port->portState = Net::SocketFactory::PS_CLOSED;
						break;
					case 8:
						port->portState = Net::SocketFactory::PS_CLOSE_WAIT;
						break;
					case 9:
						port->portState = Net::SocketFactory::PS_LAST_ACK;
						break;
					case 10:
						port->portState = Net::SocketFactory::PS_LISTEN;
						break;
					case 11:
						port->portState = Net::SocketFactory::PS_CLOSING;
						break;
					default:
						port->portState = Net::SocketFactory::PS_UNKNOWN;
						break;
					}
					port->processId = 0;
					port->socketId = Text::StrToUInt32(sarr[9]);
					portInfoList->Add(port);
					ret++;
				}
			}
		}
	}
	return ret;
}

UOSInt Net::OSSocketFactory::QueryPortInfos(NN<Data::ArrayListNN<Net::SocketFactory::PortInfo>> portInfoList, ProtocolType protoType, UInt16 procId)
{
	UOSInt retCnt = 0;
	if (protoType & Net::SocketFactory::PT_TCP)
	{
		retCnt += OSSocketFactory_LoadPortInfo(portInfoList, CSTR("/proc/net/tcp"), PT_TCP);
	}
	if (protoType & Net::SocketFactory::PT_UDP)
	{
		retCnt += OSSocketFactory_LoadPortInfo(portInfoList, CSTR("/proc/net/udp"), PT_UDP);
	}
	if (protoType & Net::SocketFactory::PT_RAW)
	{
		retCnt += OSSocketFactory_LoadPortInfo(portInfoList, CSTR("/proc/net/raw"), PT_RAW);
	}
	return retCnt;
}

void Net::OSSocketFactory::FreePortInfos(NN<Data::ArrayListNN<Net::SocketFactory::PortInfo>> portInfoList)
{
	portInfoList->MemFreeAll();
}

UOSInt Net::OSSocketFactory::QueryPortInfos2(NN<Data::ArrayListNN<Net::SocketFactory::PortInfo3>> portInfoList, ProtocolType protoType, UInt16 procId)
{
	UOSInt retCnt = 0;
	if (protoType & Net::SocketFactory::PT_TCP)
	{
		retCnt += OSSocketFactory_LoadPortInfov4(portInfoList, CSTR("/proc/net/tcp"), PT_TCP);
	}
	if (protoType & Net::SocketFactory::PT_TCP6)
	{
		retCnt += OSSocketFactory_LoadPortInfov6(portInfoList, CSTR("/proc/net/tcp6"), PT_TCP6);
	}
	if (protoType & Net::SocketFactory::PT_UDP)
	{
		retCnt += OSSocketFactory_LoadPortInfov4(portInfoList, CSTR("/proc/net/udp"), PT_UDP);
	}
	if (protoType & Net::SocketFactory::PT_UDP6)
	{
		retCnt += OSSocketFactory_LoadPortInfov6(portInfoList, CSTR("/proc/net/udp6"), PT_UDP6);
	}
	if (protoType & Net::SocketFactory::PT_RAW)
	{
		retCnt += OSSocketFactory_LoadPortInfov4(portInfoList, CSTR("/proc/net/raw"), PT_RAW);
	}
	if (protoType & Net::SocketFactory::PT_RAW6)
	{
		retCnt += OSSocketFactory_LoadPortInfov6(portInfoList, CSTR("/proc/net/raw6"), PT_RAW6);
	}
	return retCnt;
}

void Net::OSSocketFactory::FreePortInfos2(NN<Data::ArrayListNN<Net::SocketFactory::PortInfo3>> portInfoList)
{
	portInfoList->MemFreeAll();
}

Bool Net::OSSocketFactory::AdapterSetHWAddr(Text::CStringNN adapterName, UnsafeArray<const UInt8> hwAddr)
{
#if !defined(__APPLE__) && !defined(__FreeBSD__)
	int sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
	if (sock != -1)
	{
		Bool succ = false;
		struct ifreq ifr;
		struct ifreq ifrAddr;
		Text::StrConcatC(ifr.ifr_ifrn.ifrn_name, (const Char*)adapterName.v.Ptr(), adapterName.leng);
		if (ioctl(sock, SIOCGIFFLAGS, &ifr) < 0)
		{
//			printf("Sockf: Error in getting flags\r\n");
			close(sock);
			return false;
		}
		ifr.ifr_ifru.ifru_flags &= ~IFF_UP;
		if (ioctl(sock, SIOCSIFFLAGS, &ifr) < 0)
		{
//			printf("Sockf: Error in setting flags (down)\r\n");
			close(sock);
			return false;
		}

		Text::StrConcatC(ifrAddr.ifr_ifrn.ifrn_name, (const Char*)adapterName.v.Ptr(), adapterName.leng);
		MemClear(&ifrAddr.ifr_ifru.ifru_hwaddr, sizeof(sockaddr));
		ifrAddr.ifr_ifru.ifru_hwaddr.sa_family = ARPHRD_ETHER;
		MemCopyNO(ifrAddr.ifr_ifru.ifru_hwaddr.sa_data, hwAddr.Ptr(), 6);
		if (ioctl(sock, SIOCSIFHWADDR, &ifrAddr) < 0)
		{
//			printf("Sockf: Error in setting hw addr\r\n");
			succ = false;
		}
		else
		{
			succ = true;
		}
		
		ifr.ifr_ifru.ifru_flags |= IFF_UP | IFF_RUNNING;
		if (ioctl(sock, SIOCSIFFLAGS, &ifr) < 0)
		{
//			printf("Sockf: Error in setting flags (up)\r\n");
			close(sock);
			return succ;
		}
		close(sock);
		return succ;
	}
	else
	{
		return false;
	}
#else
	return false;
#endif
}

Bool Net::OSSocketFactory::AdapterEnable(Text::CStringNN adapterName, Bool enable)
{
#if !defined(__APPLE__) && !defined(__FreeBSD__)
	int sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
	if (sock != -1)
	{
		Bool succ = false;
		struct ifreq ifr;
		Text::StrConcatC(ifr.ifr_ifrn.ifrn_name, (const Char*)adapterName.v.Ptr(), adapterName.leng);
		if (ioctl(sock, SIOCGIFFLAGS, &ifr) < 0)
		{
//			printf("Sockf: Error in getting flags\r\n");
			close(sock);
			return false;
		}
		if (enable)
		{
			ifr.ifr_ifru.ifru_flags |= IFF_UP | IFF_RUNNING;
		}
		else
		{
			ifr.ifr_ifru.ifru_flags &= ~IFF_UP;
		}
		if (ioctl(sock, SIOCSIFFLAGS, &ifr) < 0)
		{
			succ = false;
		}
		else
		{
			succ = true;
		}
		close(sock);
		return succ;
	}
	else
	{
		return false;
	}
#else
	return false;
#endif
}

UOSInt Net::OSSocketFactory::GetBroadcastAddrs(NN<Data::ArrayList<UInt32>> addrs)
{
	return 0;
}

