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
#include "Text/TextEnc/Punycode.h"
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
/*	const UTF8Char *fileName = (const UTF8Char*)"/proc/sys/net/ipv6/bindv6only";
	if (IO::Path::GetPathType(fileName) == IO::Path::PT_FILE)
	{
		IO::FileStream *fs;
		NEW_CLASS(fs, IO::FileStream(fileName, IO::FileStream::FILE_MODE_CREATEWRITE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
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
	SDEL_CLASS(this->dnsHdlr);
}

UInt32 *Net::OSSocketFactory::CreateTCPSocketv4()
{
	int s = socket(AF_INET, SOCK_STREAM, 0) + 1;
	return (UInt32*)(OSInt)s;
}

UInt32 *Net::OSSocketFactory::CreateTCPSocketv6()
{
	int s = socket(AF_INET6, SOCK_STREAM, 0) + 1;
	return (UInt32*)(OSInt)s;
}

UInt32 *Net::OSSocketFactory::CreateUDPSocketv4()
{
	int s = socket(AF_INET, SOCK_DGRAM, 0) + 1;
	return (UInt32*)(OSInt)s;
}

UInt32 *Net::OSSocketFactory::CreateUDPSocketv6()
{
	int s = socket(AF_INET6, SOCK_DGRAM, 0) + 1;
	return (UInt32*)(OSInt)s;
}
UInt32 *Net::OSSocketFactory::CreateICMPIPv4Socket(UInt32 ip)
{
	int s = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP) + 1;
	return (UInt32*)(OSInt)s;
}

UInt32 *Net::OSSocketFactory::CreateUDPRAWv4Socket(UInt32 ip)
{
	int s = socket(AF_INET, SOCK_RAW, IPPROTO_UDP) + 1;
	return (UInt32*)(OSInt)s;
}

UInt32 *Net::OSSocketFactory::CreateRAWIPv4Socket(UInt32 ip)
{
	int s = socket(AF_INET, SOCK_RAW, IPPROTO_IP) + 1;
	return (UInt32*)(OSInt)s;
}

UInt32 *Net::OSSocketFactory::CreateARPSocket()
{
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
	return (UInt32*)(OSInt)s;
}

UInt32 *Net::OSSocketFactory::CreateRAWSocket()
{
#if defined(__APPLE__) || defined(__FreeBSD__)
	return 0;
#else
	int s = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL)) + 1;
	return (UInt32*)(OSInt)s;
#endif
}

void Net::OSSocketFactory::DestroySocket(UInt32 *socket)
{
	shutdown(-1 + (int)(OSInt)socket, SHUT_RDWR);
	close(-1 + (int)(OSInt)socket);
}

Bool Net::OSSocketFactory::SocketIsInvalid(UInt32 *socket)
{
	return (int)(OSInt)socket == 0;
}

Bool Net::OSSocketFactory::SocketBindv4(UInt32 *socket, UInt32 ip, UInt16 port)
{
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ip;
	addr.sin_port = htons(port);
	return bind(-1 + (int)(OSInt)socket, (sockaddr*)&addr, sizeof(addr)) != -1;
}

Bool Net::OSSocketFactory::SocketBind(UInt32 *socket, const Net::SocketUtil::AddressInfo *addr, UInt16 port)
{
	if (addr == 0 || addr->addrType == Net::SocketUtil::AT_IPV6)
	{
		UInt8 addrBuff[28];
		*(Int16*)&addrBuff[0] = AF_INET6;
		WriteMInt16(&addrBuff[2], port);
		WriteMInt32(&addrBuff[4], 0);
		if (addr == 0)
		{
			MemClear(&addrBuff[8], 20);
		}
		else
		{
			MemCopyNO(&addrBuff[8], addr->addr, 20);
		}
		return bind(-1 + (int)(OSInt)socket, (sockaddr*)&addrBuff, 28) != -1;
	}
	else if (addr->addrType == Net::SocketUtil::AT_IPV4)
	{
		sockaddr_in saddr;
		saddr.sin_family = AF_INET;
		saddr.sin_addr.s_addr = *(in_addr_t*)addr->addr;
		saddr.sin_port = htons(port);
		return bind(-1 + (int)(OSInt)socket, (sockaddr*)&saddr, sizeof(saddr)) != -1;
	}
	else
	{
		return false;
	}
}

Bool Net::OSSocketFactory::SocketListen(UInt32 *socket)
{
	return listen(-1 + (int)(OSInt)socket, SOMAXCONN) != -1;
}

UInt32 *Net::OSSocketFactory::SocketAccept(UInt32 *socket)
{
	sockaddr_in saddr;
	socklen_t addrlen = sizeof(saddr);
	int s;
	s = accept(-1 + (int)(OSInt)socket, (sockaddr*)&saddr, &addrlen) + 1;
	return (UInt32*)(OSInt)s;
}

Int32 Net::OSSocketFactory::SocketGetLastError()
{
	return errno;
}

Bool Net::OSSocketFactory::GetRemoteAddr(UInt32 *socket, Net::SocketUtil::AddressInfo *addr, UInt16 *port)
{
	sockaddr_storage addrBuff;
	sockaddr *saddr = (sockaddr*)&addrBuff;
	socklen_t size = sizeof(addrBuff);
	if (getpeername(-1 + (int)(OSInt)socket, saddr, &size) == 0)
	{
		if (saddr->sa_family == AF_INET)
		{
			if (addr)
			{
				addr->addrType = Net::SocketUtil::AT_IPV4;
				*(in_addr_t*)addr->addr = ((sockaddr_in*)&addrBuff)->sin_addr.s_addr;
			}
			if (port)
			{
				*port = ntohs(((sockaddr_in*)&addrBuff)->sin_port);
			}
			return true;
		}
		else if (saddr->sa_family == AF_INET6)
		{
			if (addr)
			{
				addr->addrType = Net::SocketUtil::AT_IPV6;
				MemCopyNO(addr->addr, &saddr->sa_data[6], 20);
			}
			if (port)
			{
				*port = ntohs(((sockaddr_in6*)&addrBuff)->sin6_port);
			}
			return true;
		}
		return false;
	}
	else
	{
		return false;
	}
}

Bool Net::OSSocketFactory::GetLocalAddr(UInt32 *socket, Net::SocketUtil::AddressInfo *addr, UInt16 *port)
{
	sockaddr_storage addrBuff;
	sockaddr *saddr = (sockaddr*)&addrBuff;
	socklen_t size = sizeof(addrBuff);
	if (getsockname(-1 + (int)(OSInt)socket, saddr, &size) == 0)
	{
		if (saddr->sa_family == AF_INET)
		{
			if (addr)
			{
				addr->addrType = Net::SocketUtil::AT_IPV4;
				*(in_addr_t*)addr->addr = ((sockaddr_in*)&addrBuff)->sin_addr.s_addr;
			}
			if (port)
			{
				*port = ntohs(((sockaddr_in*)&addrBuff)->sin_port);
			}
			return true;
		}
		else if (saddr->sa_family == AF_INET6)
		{
			if (addr)
			{
				addr->addrType = Net::SocketUtil::AT_IPV6;
				MemCopyNO(addr->addr, &saddr->sa_data[6], 20);
			}
			if (port)
			{
				*port = ntohs(((sockaddr_in6*)&addrBuff)->sin6_port);
			}
			return true;
		}
		return false;
	}
	else
	{
		return false;
	}
}

void Net::OSSocketFactory::SetDontLinger(UInt32 *socket, Bool val)
{
	linger ling;
	socklen_t leng = sizeof(ling);
	Int32 ret = getsockopt(-1 + (int)(OSInt)socket, SOL_SOCKET, SO_LINGER, &ling, &leng);
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
	ret = setsockopt(-1 + (int)(OSInt)socket, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling));
	if (ret != 0)
	{
	}
}

void Net::OSSocketFactory::SetLinger(UInt32 *socket, Int32 ms)
{
	linger ling;
	ling.l_onoff = 1;
	ling.l_linger = ms / 1000;
	setsockopt(-1 + (int)(OSInt)socket, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling));
}

void Net::OSSocketFactory::SetRecvBuffSize(UInt32 *socket, Int32 buffSize)
{
	setsockopt(-1 + (int)(OSInt)socket, SOL_SOCKET, SO_RCVBUF, &buffSize, sizeof(Int32));
}

void Net::OSSocketFactory::SetNoDelay(UInt32 *socket, Bool val)
{
	int v = val?1:0;
	setsockopt(-1 + (int)(OSInt)socket, IPPROTO_TCP, TCP_NODELAY, (char*)&v, sizeof(v));
}

void Net::OSSocketFactory::SetRecvTimeout(UInt32 *socket, Int32 ms)
{
	struct timeval tv;
	tv.tv_sec = ms / 1000;
	tv.tv_usec = (ms % 1000) * 1000;
	if (setsockopt(-1 + (int)(OSInt)socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(tv)))
	{
		printf("Error in set SO_RCVTIMEO\r\n");
	}
}

void Net::OSSocketFactory::SetReuseAddr(UInt32 *socket, Bool val)
{
	int v = val?1:0;
	setsockopt(-1 + (int)(OSInt)socket, SOL_SOCKET, SO_REUSEADDR, (char*)&v, sizeof(v));
}

void Net::OSSocketFactory::SetIPv4TTL(UInt32 *socket, Int32 ttl)
{
	setsockopt(-1 + (int)(OSInt)socket, IPPROTO_IP, IP_TTL, (char*)&ttl, sizeof(ttl));
}

void Net::OSSocketFactory::SetBroadcast(UInt32 *socket, Bool val)
{
	int v = val?1:0;
	setsockopt(-1 + (int)(OSInt)socket, SOL_SOCKET, SO_BROADCAST, (char*)&v, sizeof(v));
}

void Net::OSSocketFactory::AddIPMembership(UInt32 *socket, UInt32 ip)
{
	struct ip_mreq mreq;
	mreq.imr_multiaddr.s_addr = ip;
	mreq.imr_interface.s_addr = 0;
	setsockopt(-1 + (int)(OSInt)socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq));
}

UOSInt Net::OSSocketFactory::SendData(UInt32 *socket, const UInt8 *buff, UOSInt buffSize, ErrorType *et)
{
	int flags = 0;
#if defined(MSG_NOSIGNAL)
	flags = MSG_NOSIGNAL;
#endif
	OSInt ret = send(-1 + (int)(OSInt)socket, (const char*)buff, (size_t)buffSize, flags);
	if (ret == -1)
	{
		if (et)
		{
			if (errno == ECONNRESET)
			{
				*et = ET_CONN_RESET;
			}
			else if (errno == ECONNRESET)
			{
				*et = ET_CONN_ABORT;
			}
			else if (errno == ENOTCONN)
			{
				*et = ET_DISCONNECT;
			}
			else if (errno == ENOBUFS)
			{
				*et = ET_SHUTDOWN;
			}
			else
			{
				*et = ET_UNKNOWN;
			}
		}
		return 0;
	}
	else
	{
		if (et)
		{
			*et = ET_NO_ERROR;
		}
		return (UOSInt)ret;
	}
}

UOSInt Net::OSSocketFactory::ReceiveData(UInt32 *socket, UInt8 *buff, UOSInt buffSize, ErrorType *et)
{
	OSInt ret = recv(-1 + (int)(OSInt)socket, (char*)buff, (size_t)buffSize, 0);
//	OSInt ret = read(-1 + (int)(OSInt)socket, (char*)buff, (int)buffSize);
	if (ret == -1)
	{
		if (et)
		{
			if (errno == ECONNRESET)
			{
				*et = ET_CONN_RESET;
			}
			else if (errno == ECONNRESET)
			{
				*et = ET_CONN_ABORT;
			}
			else if (errno == ENOTCONN)
			{
				*et = ET_DISCONNECT;
			}
			else if (errno == ENOBUFS)
			{
				*et = ET_SHUTDOWN;
			}
			else
			{
				*et = ET_UNKNOWN;
			}
		}
		return 0;
	}
	else
	{
		if (et)
		{
			*et = ET_NO_ERROR;
		}
		return (UOSInt)ret;
	}
}

void *Net::OSSocketFactory::BeginReceiveData(UInt32 *socket, UInt8 *buff, UOSInt buffSize, Sync::Event *evt, ErrorType *et)
{
	UOSInt ret = ReceiveData(socket, buff, buffSize, et);
	if (ret)
	{
		evt->Set();
	}
	return (void*)ret;
}

UOSInt Net::OSSocketFactory::EndReceiveData(void *reqData, Bool toWait, Bool *incomplete)
{
	*incomplete = false;
	return (UOSInt)reqData;
}

void Net::OSSocketFactory::CancelReceiveData(void *reqData)
{
}

UOSInt Net::OSSocketFactory::UDPReceive(UInt32 *socket, UInt8 *buff, UOSInt buffSize, Net::SocketUtil::AddressInfo *addr, UInt16 *port, ErrorType *et)
{
	OSInt recvSize;
	sockaddr_storage addrBuff;
	sockaddr *saddr = (sockaddr*)&addrBuff;
	socklen_t size = sizeof(addrBuff);
	saddr->sa_family = 0;
	recvSize = recvfrom(-1 + (int)(OSInt)socket, (Char*)buff, (size_t)buffSize, 0, saddr, (socklen_t*)&size);
	if (recvSize <= 0)
	{
		if (et)
		{
			if (errno == ECONNRESET)
			{
				*et = ET_CONN_RESET;
			}
			else if (errno == ECONNRESET)
			{
				*et = ET_CONN_ABORT;
			}
			else if (errno == ENOTCONN)
			{
				*et = ET_DISCONNECT;
			}
			else if (errno == ENOBUFS)
			{
				*et = ET_SHUTDOWN;
			}
			else
			{
				*et = ET_UNKNOWN;
			}
		}
		return 0;
	}
	else
	{
		if (saddr->sa_family == AF_INET)
		{
			addr->addrType = Net::SocketUtil::AT_IPV4;
			*(in_addr_t*)addr->addr = ((sockaddr_in*)&addrBuff)->sin_addr.s_addr;
			*port = ntohs(((sockaddr_in*)&addrBuff)->sin_port);
			return (UOSInt)recvSize;
		}
		else if (saddr->sa_family == AF_INET6)
		{
			addr->addrType = Net::SocketUtil::AT_IPV6;
			MemCopyNO(addr->addr, &saddr->sa_data[6], 20);
			*port = ntohs(((sockaddr_in6*)&addrBuff)->sin6_port);
			return (UOSInt)recvSize;
		}
		else
		{
//			printf("UDPReceive: unknown family %d\r\n", saddr->sa_family);
		}
		
		return (UOSInt)recvSize;
	}
}

UOSInt Net::OSSocketFactory::SendTo(UInt32 *socket, const UInt8 *buff, UOSInt buffSize, const Net::SocketUtil::AddressInfo *addr, UInt16 port)
{
	sockaddr_storage addrBase;
	sockaddr *addrBuff = (sockaddr*)&addrBase;
	socklen_t addrSize;
	if (addr == 0)
	{
		MemClear(addrBuff, sizeof(sockaddr));
		addrBuff->sa_family = AF_INET;
		addrSize = sizeof(sockaddr);
	}
	else if (addr->addrType == Net::SocketUtil::AT_IPV4)
	{
		sockaddr_in *saddr = (sockaddr_in*)&addrBase;
		saddr->sin_family = AF_INET;
		saddr->sin_addr.s_addr = *(in_addr_t*)addr->addr;
		saddr->sin_port = htons(port);
		addrSize = sizeof(sockaddr_in);
	}
	else if (addr->addrType == Net::SocketUtil::AT_IPV6)
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
	OSInt ret = sendto(-1 + (int)(OSInt)socket, (const char*)buff, (size_t)buffSize, 0, addrBuff, addrSize);
	if (ret == -1)
	{
		return 0;
	}
	else
	{
		return (UOSInt)ret;
	}
}

UOSInt Net::OSSocketFactory::SendToIF(UInt32 *socket, const UInt8 *buff, UOSInt buffSize, const UTF8Char *ifName)
{
	sockaddr addrBase;
	sockaddr *addrBuff = (sockaddr*)&addrBase;
	socklen_t addrSize;
	MemClear(addrBuff, sizeof(sockaddr));
	addrBuff->sa_family = AF_INET;
	Text::StrConcat(addrBuff->sa_data, (const Char*)ifName);
	addrSize = sizeof(sockaddr);

	OSInt ret = sendto(-1 + (int)(OSInt)socket, (const char*)buff, (size_t)buffSize, 0, addrBuff, addrSize);
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

Bool Net::OSSocketFactory::IcmpSendEcho2(const Net::SocketUtil::AddressInfo *addr, Int32 *respTime_us, Int32 *ttl)
{
	int rs;
	if (addr->addrType == Net::SocketUtil::AT_IPV4)
	{
		rs = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP);
	}
	else if (addr->addrType == Net::SocketUtil::AT_IPV6)
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
		if (addr->addrType == Net::SocketUtil::AT_IPV4)
		{
			sockaddr_in *saddr = (sockaddr_in*)&addrBase;
			saddr->sin_family = AF_INET;
			saddr->sin_addr.s_addr = *(in_addr_t*)addr->addr;
			saddr->sin_port = 0;
			addrSize = sizeof(sockaddr_in);
		}
		else if (addr->addrType == Net::SocketUtil::AT_IPV6)
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
			*ttl = iphdrptr->ttl;
			*respTime_us = (Int32)timeStart;
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
		UTF8Char *sptr;
		if (addr->addrType == Net::SocketUtil::AT_IPV6)
		{
			sptr = Text::StrConcat(sbuff, (const UTF8Char*)"ping6 -c 1 ");
		}
		else
		{
			sptr = Text::StrConcat(sbuff, (const UTF8Char*)"ping -c 1 ");
		}
		Net::SocketUtil::GetAddrName(sptr, addr);
		Text::StringBuilderUTF8 sb;
		Int32 ret;
		ret = Manage::Process::ExecuteProcess(sbuff, &sb);
		if (ret == 0)
		{
			UTF8Char *sarr[4];
			UOSInt ui = Text::StrSplitLine(sarr, 3, sb.ToString());
			if (ui == 3)
			{
				OSInt si = Text::StrIndexOf(sarr[1], (const UTF8Char*)": ");
				if (si >= 0)
				{
					sarr[1] = &sarr[1][si + 2];
				}
				if (Text::StrStartsWith(sarr[1], (const UTF8Char*)"icmp_seq="))
				{
					if (Text::StrSplitWS(sarr, 4, sarr[1]) == 4)
					{
						*ttl = Text::StrToInt32(&sarr[1][4]);
						*respTime_us = Math::Double2Int32(Text::StrToDouble(&sarr[2][5]) * 1000.0);
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

Bool Net::OSSocketFactory::Connect(UInt32 *socket, UInt32 ip, UInt16 port)
{
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ip;
	addr.sin_port = htons(port);

/*	fd_set fdset;
	struct timeval tv;

	Bool succ = false;
	long arg = fcntl(-1 + (int)(OSInt)socket, F_GETFL, 0);
    fcntl(-1 + (int)(OSInt)socket, F_SETFL, arg | O_NONBLOCK);

    connect(-1 + (int)(OSInt)socket, (struct sockaddr *)&addr, sizeof(addr));

    FD_ZERO(&fdset);
    FD_SET(-1 + (int)(OSInt)socket, &fdset);
    tv.tv_sec = 10;
    tv.tv_usec = 0;

    if (select((int)(OSInt)socket, NULL, &fdset, NULL, &tv) == 1) //sock + 1
    {
        int so_error;
        socklen_t len = sizeof so_error;

        getsockopt(-1 + (int)(OSInt)socket, SOL_SOCKET, SO_ERROR, &so_error, &len);

        if (so_error == 0)
		{
			succ = true;
        }
    }

    fcntl(-1 + (int)(OSInt)socket, F_SETFL, arg);

	return succ;*/
    return connect(-1 + (int)(OSInt)socket, (struct sockaddr *)&addr, sizeof(addr)) == 0;
}

Bool Net::OSSocketFactory::Connect(UInt32 *socket, const Net::SocketUtil::AddressInfo *addr, UInt16 port)
{
	UInt8 addrBuff[28];
	socklen_t socklen;
	if (addr->addrType == Net::SocketUtil::AT_IPV4)
	{
		*(Int16*)&addrBuff[0] = AF_INET;
		WriteMInt16(&addrBuff[2], port);
		*(Int32*)&addrBuff[4] = *(Int32*)addr->addr;
		socklen = sizeof(sockaddr_in);
	}
	else if (addr->addrType == Net::SocketUtil::AT_IPV6)
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
	return connect(-1 + (int)(OSInt)socket, (const sockaddr*)addrBuff, socklen) == 0;
}

void Net::OSSocketFactory::ShutdownSend(UInt32 *socket)
{
	shutdown(-1 + (int)(OSInt)socket, SHUT_WR);
}

Bool Net::OSSocketFactory::SocketGetReadBuff(UInt32 *socket, UInt32 *size)
{
/*	struct timeval tv;
	fd_set readset;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	FD_ZERO(&readset);
	FD_SET(-1 + (int)(OSInt)socket, &readset);
	int result = select(1, &readset, 0, 0, &tv);
	Bool ret = false;
	if (result <= 0)
	{
		*size = 0;
	}
	else if (FD_ISSET(-1 + (int)(OSInt)socket, &readset))
	{
		ret = true;
		*size = 1;
	}*/

	return ioctl(-1 + (int)(OSInt)socket, FIONREAD, (int*)size) == 0;
}

Bool Net::OSSocketFactory::DNSResolveIPDef(const Char *host, Net::SocketUtil::AddressInfo *addr)
{
	addrinfo *result = 0;
	Int32 iResult = getaddrinfo(host, 0, 0, &result);
	if (iResult == 0)
	{
		Bool succ = false;
		if (result->ai_addr->sa_family == AF_INET)
		{
			addr->addrType = Net::SocketUtil::AT_IPV4;
			*(in_addr_t*)addr->addr = ((sockaddr_in*)result->ai_addr)->sin_addr.s_addr;
			succ = true;
		}
		else if (result->ai_addr->sa_family == AF_INET6)
		{
			addr->addrType = Net::SocketUtil::AT_IPV6;
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

Bool Net::OSSocketFactory::GetDefDNS(Net::SocketUtil::AddressInfo *addr)
{
	Text::UTF8Reader *reader;
	IO::FileStream *fs;
	NEW_CLASS(fs, IO::FileStream((const UTF8Char*)"/etc/resolv.conf", IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	if (fs->IsError())
	{
		DEL_CLASS(fs);
		return false;
	}
	Bool ret = false;
	UTF8Char *sarr[3];
	Text::StringBuilderUTF8 sb;
	NEW_CLASS(reader, Text::UTF8Reader(fs));
	while (true)
	{
		sb.ClearStr();
		if (!reader->ReadLine(&sb, 1024))
		{
			break;
		}
		sb.Trim();
		if (sb.StartsWith((const UTF8Char*)"#"))
		{
		}
		else
		{
			if (Text::StrSplitTrim(sarr, 3, sb.ToString(), ' ') == 2)
			{
				if (Text::StrEquals(sarr[0], (const UTF8Char*)"nameserver"))
				{
					if (Net::SocketUtil::GetIPAddr(sarr[1], addr))
					{
						ret = true;
						break;
					}
				}
			}
		}
	}

	DEL_CLASS(reader);
	DEL_CLASS(fs);
	return ret;
}

UOSInt Net::OSSocketFactory::GetDNSList(Data::ArrayList<UInt32> *dnsList)
{
	Text::UTF8Reader *reader;
	IO::FileStream *fs;
	NEW_CLASS(fs, IO::FileStream((const UTF8Char*)"/etc/resolv.conf", IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	if (fs->IsError())
	{
		DEL_CLASS(fs);
		return false;
	}
	UOSInt ret = 0;
	UTF8Char *sarr[3];
	Text::StringBuilderUTF8 sb;
	Net::SocketUtil::AddressInfo addr;
	NEW_CLASS(reader, Text::UTF8Reader(fs));
	while (true)
	{
		sb.ClearStr();
		if (!reader->ReadLine(&sb, 1024))
		{
			break;
		}
		sb.Trim();
		if (sb.StartsWith((const UTF8Char*)"#"))
		{
		}
		else
		{
			if (Text::StrSplitTrim(sarr, 3, sb.ToString(), ' ') == 2)
			{
				if (Text::StrEquals(sarr[0], (const UTF8Char*)"nameserver"))
				{
					if (Net::SocketUtil::GetIPAddr(sarr[1], &addr))
					{
						if (addr.addrType == Net::SocketUtil::AT_IPV4)
						{
							ret++;
							dnsList->Add(*(UInt32*)addr.addr);
						}
					}
				}
			}
		}
	}

	DEL_CLASS(reader);
	DEL_CLASS(fs);
	return ret;
}

Bool Net::OSSocketFactory::LoadHosts(Net::DNSHandler *dnsHdlr)
{
	Text::UTF8Reader *reader;
	IO::FileStream *fs;
	Net::SocketUtil::AddressInfo addr;
	UOSInt i;
	UTF8Char *sarr[2];
	NEW_CLASS(fs, IO::FileStream((const UTF8Char*)"/etc/hosts", IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	if (fs->IsError())
	{
		DEL_CLASS(fs);
		return false;
	}
	NEW_CLASS(reader, Text::UTF8Reader(fs));
	Text::StringBuilderUTF8 sb;
	while (reader->ReadLine(&sb, 512))
	{
		sb.Trim();
		if (sb.ToString()[0] == '#')
		{
		}
		else
		{
			i = Text::StrSplitWS(sarr, 2, sb.ToString());
			if (i == 2)
			{
				if (Net::SocketUtil::GetIPAddr(sarr[0], &addr))
				{
					while (true)
					{
						i = Text::StrSplitWS(sarr, 2, sarr[1]);
						dnsHdlr->AddHost(&addr, sarr[0]);
						if (i != 2)
							break;
					}
				}
			}
		}

		sb.ClearStr();
	}
	DEL_CLASS(reader);
	DEL_CLASS(fs);
	return true;
}

Bool Net::OSSocketFactory::ARPAddRecord(UOSInt ifIndex, const UInt8 *hwAddr, UInt32 ipv4)
{
	return false;
}


UOSInt Net::OSSocketFactory::GetConnInfoList(Data::ArrayList<Net::ConnectionInfo*> *connInfoList)
{
	int sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
	if (sock == -1)
		return 0;

	Net::ConnectionInfo *connInfo;
	ConnectionData data;
	UOSInt ret = 0;
	Char buff[1024];
	ifconf ifc;
	ifreq *ifrend;
	ifreq *ifrcurr;

	ifc.ifc_len = sizeof(buff);
	ifc.ifc_buf = buff;
	if (ioctl(sock, SIOCGIFCONF, &ifc) >= 0)
	{
		ifrcurr = ifc.ifc_req;
		ifrend = ifrcurr + ((UOSInt)ifc.ifc_len / sizeof(ifreq));
		while (ifrcurr != ifrend)
		{
			data.sock = sock;
			data.name = ifrcurr->ifr_name;
			NEW_CLASS(connInfo, Net::ConnectionInfo(&data));
			connInfoList->Add(connInfo);
			ret++;
			ifrcurr++;
		}
	}
	close(sock);

	UTF8Char sbuff[128];
	IO::FileStream *fs;
	Text::UTF8Reader *reader;
	Text::StringBuilderUTF8 sb;
	UInt32 ip;
	UInt32 gw;
	UOSInt i;
	UTF8Char *sarr[4];
	NEW_CLASS(fs, IO::FileStream((const UTF8Char*)"/proc/net/route", IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	if (!fs->IsError())
	{
		NEW_CLASS(reader, Text::UTF8Reader(fs));
		if (reader->ReadLine(&sb, 512))
		{
			sb.ClearStr();
			while (reader->ReadLine(&sb, 512))
			{
				i = Text::StrSplitWS(sarr, 4, sb.ToString());
				if (i == 4)
				{
					ip = Text::StrHex2UInt32(sarr[1]);
					gw = Text::StrHex2UInt32(sarr[2]);
					if (ip == 0 && gw != 0)
					{
						i = connInfoList->GetCount();
						while (i-- > 0)
						{
							connInfo = connInfoList->GetItem(i);
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
		DEL_CLASS(reader);
	}
	DEL_CLASS(fs);	
	return ret;
}

Bool Net::OSSocketFactory::GetIPInfo(IPInfo *info)
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

Bool Net::OSSocketFactory::GetTCPInfo(TCPInfo *info)
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

Bool Net::OSSocketFactory::GetUDPInfo(UDPInfo *info)
{
	info->dwInDatagrams = 0;
	info->dwNoPorts = 0;
	info->dwInErrors = 0;
	info->dwOutDatagrams = 0;
	info->dwNumAddrs = 0;
	return true;
}

UOSInt OSSocketFactory_LoadPortInfo(Data::ArrayList<Net::SocketFactory::PortInfo*> *portInfoList, const UTF8Char *path, Net::SocketFactory::ProtocolType protoType)
{
	UOSInt ret = 0;
	IO::FileStream *fs;
	Text::UTF8Reader *reader;
	Net::SocketFactory::PortInfo *port;
	NEW_CLASS(fs, IO::FileStream(path, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	if (fs->IsError())
	{
		
	}
	else
	{
		UTF8Char *sarr[5];
		UTF8Char *sarr2[3];
		Text::StringBuilderUTF8 sb;
		NEW_CLASS(reader, Text::UTF8Reader(fs));
		if (reader->ReadLine(&sb, 1024))
		{
			while (true)
			{
				sb.ClearStr();
				if (!reader->ReadLine(&sb, 1024))
				{
					break;
				}
				sb.Trim();
				if (Text::StrSplitWS(sarr, 5, sb.ToString()) == 5)
				{
					port = MemAlloc(Net::SocketFactory::PortInfo, 1);
					port->protoType = protoType;
					if (Text::StrSplit(sarr2, 3, sarr[1], ':') == 2)
					{
						port->localIP = (UInt32)Text::StrHex2Int32(sarr2[0]);
						port->localPort = (UInt16)Text::StrHex2Int16(sarr2[1]);	
					}
					else
					{
						port->localIP = 0;
						port->localPort = 0;
					}
					if (Text::StrSplit(sarr2, 3, sarr[2], ':') == 2)
					{
						port->foreignIP = (UInt32)Text::StrHex2Int32(sarr2[0]);
						port->foreignPort = (UInt16)Text::StrHex2Int16(sarr2[1]);	
					}
					else
					{
						port->foreignIP = 0;
						port->foreignPort = 0;
					}
					switch (Text::StrHex2Int32(sarr[3]))
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
		DEL_CLASS(reader);
	}
	DEL_CLASS(fs);
	return ret;
}

UOSInt OSSocketFactory_LoadPortInfov4(Data::ArrayList<Net::SocketFactory::PortInfo2*> *portInfoList, const UTF8Char *path, Net::SocketFactory::ProtocolType protoType)
{
	UOSInt ret = 0;
	IO::FileStream *fs;
	Text::UTF8Reader *reader;
	Net::SocketFactory::PortInfo2 *port;
	NEW_CLASS(fs, IO::FileStream(path, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	if (fs->IsError())
	{
		
	}
	else
	{
		UTF8Char *sarr[5];
		UTF8Char *sarr2[3];
		Text::StringBuilderUTF8 sb;
		NEW_CLASS(reader, Text::UTF8Reader(fs));
		if (reader->ReadLine(&sb, 1024))
		{
			while (true)
			{
				sb.ClearStr();
				if (!reader->ReadLine(&sb, 1024))
				{
					break;
				}
				sb.Trim();
				if (Text::StrSplitWS(sarr, 5, sb.ToString()) == 5)
				{
					port = MemAlloc(Net::SocketFactory::PortInfo2, 1);
					port->protoType = protoType;
					if (Text::StrSplit(sarr2, 3, sarr[1], ':') == 2)
					{
						Net::SocketUtil::SetAddrInfoV4(&port->localAddr, (UInt32)Text::StrHex2Int32(sarr2[0]));
						port->localPort = (UInt16)Text::StrHex2Int16(sarr2[1]);	
					}
					else
					{
						Net::SocketUtil::SetAddrInfoV4(&port->localAddr, 0);
						port->localPort = 0;
					}
					if (Text::StrSplit(sarr2, 3, sarr[2], ':') == 2)
					{
						Net::SocketUtil::SetAddrInfoV4(&port->foreignAddr, (UInt32)Text::StrHex2Int32(sarr2[0]));
						port->foreignPort = (UInt16)Text::StrHex2Int16(sarr2[1]);	
					}
					else
					{
						Net::SocketUtil::SetAddrInfoV4(&port->foreignAddr, 0);
						port->foreignPort = 0;
					}
					switch (Text::StrHex2Int32(sarr[3]))
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
		DEL_CLASS(reader);
	}
	DEL_CLASS(fs);
	return ret;
}

UOSInt OSSocketFactory_LoadPortInfov6(Data::ArrayList<Net::SocketFactory::PortInfo2*> *portInfoList, const UTF8Char *path, Net::SocketFactory::ProtocolType protoType)
{
	UOSInt ret = 0;
	IO::FileStream *fs;
	Text::UTF8Reader *reader;
	Net::SocketFactory::PortInfo2 *port;
	NEW_CLASS(fs, IO::FileStream(path, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	if (fs->IsError())
	{
		
	}
	else
	{
		UInt8 addr[32];
		UTF8Char *sarr[5];
		UTF8Char *sarr2[3];
		Text::StringBuilderUTF8 sb;
		NEW_CLASS(reader, Text::UTF8Reader(fs));
		if (reader->ReadLine(&sb, 1024))
		{
			while (true)
			{
				sb.ClearStr();
				if (!reader->ReadLine(&sb, 1024))
				{
					break;
				}
				sb.Trim();
				if (Text::StrSplitWS(sarr, 5, sb.ToString()) == 5)
				{
					port = MemAlloc(Net::SocketFactory::PortInfo2, 1);
					port->protoType = protoType;
					if (Text::StrSplit(sarr2, 3, sarr[1], ':') == 2)
					{
						Text::StrHex2Bytes(sarr2[0], addr);
						Net::SocketUtil::SetAddrInfoV6(&port->localAddr, addr, 0);
						port->localPort = (UInt16)Text::StrHex2Int16(sarr2[1]);	
					}
					else
					{
						MemClear(addr, 16);
						Net::SocketUtil::SetAddrInfoV6(&port->localAddr, addr, 0);
						port->localPort = 0;
					}
					if (Text::StrSplit(sarr2, 3, sarr[2], ':') == 2)
					{
						Text::StrHex2Bytes(sarr2[0], addr);
						Net::SocketUtil::SetAddrInfoV6(&port->foreignAddr, addr, 0);
						port->foreignPort = (UInt16)Text::StrHex2Int16(sarr2[1]);	
					}
					else
					{
						MemClear(addr, 16);
						Net::SocketUtil::SetAddrInfoV6(&port->foreignAddr, addr, 0);
						port->foreignPort = 0;
					}
					switch (Text::StrHex2Int32(sarr[3]))
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
		DEL_CLASS(reader);
	}
	DEL_CLASS(fs);
	return ret;
}

UOSInt Net::OSSocketFactory::QueryPortInfos(Data::ArrayList<Net::SocketFactory::PortInfo*> *portInfoList, ProtocolType protoType, UInt16 procId)
{
	UOSInt retCnt = 0;
	if (protoType & Net::SocketFactory::PT_TCP)
	{
		retCnt += OSSocketFactory_LoadPortInfo(portInfoList, (const UTF8Char*)"/proc/net/tcp", PT_TCP);
	}
	if (protoType & Net::SocketFactory::PT_UDP)
	{
		retCnt += OSSocketFactory_LoadPortInfo(portInfoList, (const UTF8Char*)"/proc/net/udp", PT_UDP);
	}
	if (protoType & Net::SocketFactory::PT_RAW)
	{
		retCnt += OSSocketFactory_LoadPortInfo(portInfoList, (const UTF8Char*)"/proc/net/raw", PT_RAW);
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

UOSInt Net::OSSocketFactory::QueryPortInfos2(Data::ArrayList<Net::SocketFactory::PortInfo2*> *portInfoList, ProtocolType protoType, UInt16 procId)
{
	UOSInt retCnt = 0;
	if (protoType & Net::SocketFactory::PT_TCP)
	{
		retCnt += OSSocketFactory_LoadPortInfov4(portInfoList, (const UTF8Char*)"/proc/net/tcp", PT_TCP);
	}
	if (protoType & Net::SocketFactory::PT_TCP6)
	{
		retCnt += OSSocketFactory_LoadPortInfov6(portInfoList, (const UTF8Char*)"/proc/net/tcp6", PT_TCP6);
	}
	if (protoType & Net::SocketFactory::PT_UDP)
	{
		retCnt += OSSocketFactory_LoadPortInfov4(portInfoList, (const UTF8Char*)"/proc/net/udp", PT_UDP);
	}
	if (protoType & Net::SocketFactory::PT_UDP6)
	{
		retCnt += OSSocketFactory_LoadPortInfov6(portInfoList, (const UTF8Char*)"/proc/net/udp6", PT_UDP6);
	}
	if (protoType & Net::SocketFactory::PT_RAW)
	{
		retCnt += OSSocketFactory_LoadPortInfov4(portInfoList, (const UTF8Char*)"/proc/net/raw", PT_RAW);
	}
	if (protoType & Net::SocketFactory::PT_RAW6)
	{
		retCnt += OSSocketFactory_LoadPortInfov6(portInfoList, (const UTF8Char*)"/proc/net/raw6", PT_RAW6);
	}
	return retCnt;
}

void Net::OSSocketFactory::FreePortInfos2(Data::ArrayList<Net::SocketFactory::PortInfo2*> *portInfoList)
{
	UOSInt i = portInfoList->GetCount();
	while (i-- > 0)
	{
		MemFree(portInfoList->RemoveAt(i));
	}
}

Bool Net::OSSocketFactory::AdapterSetHWAddr(const UTF8Char *adapterName, const UInt8 *hwAddr)
{
#if !defined(__APPLE__) && !defined(__FreeBSD__)
	int sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
	if (sock != -1)
	{
		Bool succ = false;
		struct ifreq ifr;
		struct ifreq ifrAddr;
		Text::StrConcat(ifr.ifr_ifrn.ifrn_name, (const Char*)adapterName);
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

		Text::StrConcat(ifrAddr.ifr_ifrn.ifrn_name, (const Char*)adapterName);
		MemClear(&ifrAddr.ifr_ifru.ifru_hwaddr, sizeof(sockaddr));
		ifrAddr.ifr_ifru.ifru_hwaddr.sa_family = ARPHRD_ETHER;
		MemCopyNO(ifrAddr.ifr_ifru.ifru_hwaddr.sa_data, hwAddr, 6);
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

UOSInt Net::OSSocketFactory::GetBroadcastAddrs(Data::ArrayList<UInt32> *addrs)
{
	return 0;
}

