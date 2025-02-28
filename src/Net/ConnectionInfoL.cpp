#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "Net/ConnectionInfo.h"
#include "Net/SocketFactory.h"
#include "Net/SocketUtil.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"
#include <ifaddrs.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#if defined(__sun__)
#include <sys/sockio.h>
#endif

typedef struct
{
	int sock;
	const Char *name;
} ConnectionData;

Net::ConnectionInfo::ConnectionInfo(void *info)
{
	ConnectionData *data = (ConnectionData*)info;
	ifreq ifr;
	UOSInt nameLen = Text::StrCharCntCh(data->name);
	Text::StrConcatC(ifr.ifr_name, data->name, nameLen);
	UInt8 buff[64];

	Text::StringBuilderUTF8 sb;
	sb.AppendC((const UTF8Char*)data->name, nameLen);
	this->ent.internalName = Text::StrCopyNewChC(data->name, nameLen).Ptr();
	this->ent.name = Text::StrCopyNewC(sb.ToString(), sb.GetLength()).Ptr();
	this->ent.description = Text::StrCopyNewC(sb.ToString(), sb.GetLength()).Ptr();

	if (ioctl(data->sock, SIOCGIFFLAGS, &ifr) == 0)
	{
#if defined(__FreeBSD__) || defined(__MACH__) || defined(__sun__)
		this->ent.dhcpEnabled = false;
#else
		this->ent.dhcpEnabled = (ifr.ifr_flags & IFF_DYNAMIC) != 0;
#endif
		if (ifr.ifr_flags & IFF_LOOPBACK)
		{
			this->ent.connType = Net::ConnectionInfo::ConnectionType::Loopback;
		}
		else if (ifr.ifr_flags & IFF_POINTOPOINT)
		{
			this->ent.connType = Net::ConnectionInfo::ConnectionType::DialUp;
		}
		else if (sb.StartsWith(UTF8STRC("eth")))
		{
			this->ent.connType = Net::ConnectionInfo::ConnectionType::Ethernet;
		}
		else if (sb.StartsWith(UTF8STRC("wlan")))
		{
			this->ent.connType = Net::ConnectionInfo::ConnectionType::WiFi;
		}
		else if (sb.StartsWith(UTF8STRC("wlo")))
		{
			this->ent.connType = Net::ConnectionInfo::ConnectionType::WiFi;
		}
		else if (sb.StartsWith(UTF8STRC("rmnet_data")))
		{
			this->ent.connType = Net::ConnectionInfo::ConnectionType::DialUp;
		}
		else if (sb.StartsWith(UTF8STRC("lxcbr")))
		{
			this->ent.connType = Net::ConnectionInfo::ConnectionType::Bridge;
		}
		else
		{
			this->ent.connType = Net::ConnectionInfo::ConnectionType::Unknown;
		}
		if (ifr.ifr_flags & IFF_DEBUG)
		{
			this->ent.connStatus = Net::ConnectionInfo::CS_TESTING;
		}
		else if (ifr.ifr_flags & IFF_UP)
		{
			this->ent.connStatus = Net::ConnectionInfo::CS_UP;
		}
		else
		{
			this->ent.connStatus = Net::ConnectionInfo::CS_DOWN;
		}
	}
	else
	{
		this->ent.dhcpEnabled = false;
		this->ent.connType = Net::ConnectionInfo::ConnectionType::Unknown;
		this->ent.connStatus = Net::ConnectionInfo::CS_UNKNOWN;
	}
	if (ioctl(data->sock, SIOCGIFMTU, &ifr) == 0)
	{
		this->ent.mtu = (UInt32)ifr.ifr_mtu;
	}
	else
	{
		this->ent.mtu = 0;
	}
#if defined(__APPLE__)
	this->ent.index = 0;
#else
	if (ioctl(data->sock, SIOCGIFINDEX, &ifr) == 0)
	{
		this->ent.index = (UInt32)ifr.ifr_ifru.ifru_ivalue;
	}
	else
	{
		this->ent.index = 0;
	}
#endif
#if defined(__FreeBSD__) || defined(__MACH__)
	UInt8 macAddr[6];
	ifr.ifr_ifru.ifru_data = (caddr_t)macAddr;
	if (ioctl(data->sock, SIOCGIFMAC, &ifr) == 0)
	{
		this->ent.physicalAddr = MemAlloc(UInt8, this->ent.physicalAddrLeng = 6);
		MemCopyNO(this->ent.physicalAddr, macAddr, 6);
	}
	else
#elif defined(__sun__)
	if (false)
	{
	}
	else
#else
	if (ioctl(data->sock, SIOCGIFHWADDR, &ifr) == 0)
	{
		this->ent.physicalAddr = MemAlloc(UInt8, this->ent.physicalAddrLeng = 6);
		MemCopyNO(this->ent.physicalAddr, ifr.ifr_hwaddr.sa_data, 6);
	}
	else
#endif
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("/sys/class/net/"));
		sb.AppendSlow((const UTF8Char*)data->name);
		sb.AppendC(UTF8STRC("/address"));
	
		UOSInt readSize;
		{
			IO::FileStream fs(sb.ToCString(), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			readSize = fs.Read(BYTEARR(buff).WithSize(63));
		}
		buff[readSize] = 0;
		this->ent.physicalAddr = 0;
		this->ent.physicalAddrLeng = 0;
		if (readSize > 0)
		{
			UnsafeArray<UTF8Char> sarr[7];
			readSize = Text::StrSplit(sarr, 7, (UTF8Char*)buff, ':');
			if (readSize == 6)
			{
				this->ent.physicalAddr = MemAlloc(UInt8, this->ent.physicalAddrLeng = 6);
				sarr[5][2] = 0; 
				this->ent.physicalAddr[0] = (UInt8)(Text::StrHex2Int32C(sarr[0]) & 0xff);
				this->ent.physicalAddr[1] = (UInt8)(Text::StrHex2Int32C(sarr[1]) & 0xff);
				this->ent.physicalAddr[2] = (UInt8)(Text::StrHex2Int32C(sarr[2]) & 0xff);
				this->ent.physicalAddr[3] = (UInt8)(Text::StrHex2Int32C(sarr[3]) & 0xff);
				this->ent.physicalAddr[4] = (UInt8)(Text::StrHex2Int32C(sarr[4]) & 0xff);
				this->ent.physicalAddr[5] = (UInt8)(Text::StrHex2Int32C(sarr[5]) & 0xff);
			}
		}
	}
	this->ent.dhcpSvr = 0;
	this->ent.dnsSuffix = 0;
	
#if !defined(NO_GETIFADDRS) && (!defined(__ANDROID_API__) || __ANDROID_API__ >= 24)
	struct ifaddrs *ifap;
	struct ifaddrs *ifa;
	if (getifaddrs(&ifap) == 0)
	{
		ifa = ifap;
		while (ifa)
		{
			if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET && Text::StrEqualsCh(ifa->ifa_name, data->name))
			{
				this->ent.ipaddr.Add((UInt32)((sockaddr_in*)ifa->ifa_addr)->sin_addr.s_addr);
			}
			ifa = ifa->ifa_next;
		}
		freeifaddrs(ifap);
	}
#else
#endif
	
	{
		IO::FileStream fs(CSTR("/etc/resolv.conf"), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		if (!fs.IsError())
		{
			Text::PString sarr[3];
			Text::UTF8Reader reader(fs);
			sb.ClearStr();
			while (reader.ReadLine(sb, 512))
			{
				sb.Trim();
				if (sb.ToString()[0] == '#')
				{
				}
				else if (sb.StartsWith(UTF8STRC("nameserver")))
				{
					if (Text::StrSplitWSP(sarr, 3, sb) >= 2)
					{
						this->ent.dnsaddr.Add(Net::SocketUtil::GetIPAddr(sarr[1].ToCString()));
					}
				}
				sb.ClearStr();
			}
		}
	}
	this->ent.defGW = 0;

/*	NEW_CLASS(fs, IO::FileStream(L"/proc/net/route", IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (!fs->IsError())
	{
		WChar *sarr[4];
		NEW_CLASS(reader, Text::UTF8Reader(fs));
		sb.ClearStr();
		reader->ReadLine(sb, 512);
		sb.ClearStr();
		while (reader->ReadLine(sb, 512))
		{
			if (Text::StrSplit(sarr, 4, sb.ToString(), '\t') == 4)
			{
				if (Text::StrEquals(sarr[0], this->name))
				{
					UInt32 dest = Text::StrHex2Int32C(sarr[1]);
					UInt32 gw = Text::StrHex2Int32C(sarr[2]);
					if (dest == 0 && gw != 0)
					{
						this->ent.defGW = gw;
						break;
					}
				}
			}
			sb.ClearStr();
		}
		DEL_CLASS(reader);
	}
	DEL_CLASS(fs);*/
}

Bool Net::ConnectionInfo::SetInfo(void *info)
{
	this->ent.defGW = *(UInt32*)info;
	return true;
}
