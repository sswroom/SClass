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
#include <wchar.h>

typedef struct
{
	int sock;
	const Char *name;
} ConnectionData;

Net::ConnectionInfo::ConnectionInfo(void *info)
{
	ConnectionData *data = (ConnectionData*)info;
	ifreq ifr;
	Text::StrConcat(ifr.ifr_name, data->name);
	UInt8 buff[64];
	IO::FileStream *fs;

	Text::StringBuilderUTF8 sb;
	sb.AppendSlow((const UTF8Char*)data->name);
	this->ent.internalName = Text::StrCopyNew(data->name);
	this->ent.name = Text::StrCopyNew(sb.ToString());
	this->ent.description = Text::StrCopyNew(sb.ToString());

	if (ioctl(data->sock, SIOCGIFFLAGS, &ifr) == 0)
	{
#if defined(__FreeBSD__) || defined(__MACH__) || defined(__sun__)
		this->ent.dhcpEnabled = false;
#else
		this->ent.dhcpEnabled = (ifr.ifr_flags & IFF_DYNAMIC) != 0;
#endif
		if (ifr.ifr_flags & IFF_LOOPBACK)
		{
			this->ent.connType = Net::ConnectionInfo::CT_LOOPBACK;
		}
		else if (ifr.ifr_flags & IFF_POINTOPOINT)
		{
			this->ent.connType = Net::ConnectionInfo::CT_DIALUP;
		}
		else if (Text::StrStartsWith(data->name, "eth"))
		{
			this->ent.connType = Net::ConnectionInfo::CT_ETHERNET;
		}
		else if (Text::StrStartsWith(data->name, "wlan"))
		{
			this->ent.connType = Net::ConnectionInfo::CT_WIFI;
		}
		else if (Text::StrStartsWith(data->name, "wlo"))
		{
			this->ent.connType = Net::ConnectionInfo::CT_WIFI;
		}
		else if (Text::StrStartsWith(data->name, "rmnet_data"))
		{
			this->ent.connType = Net::ConnectionInfo::CT_DIALUP;
		}
		else
		{
			this->ent.connType = Net::ConnectionInfo::CT_UNKNOWN;
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
		this->ent.connType = Net::ConnectionInfo::CT_UNKNOWN;
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
	if (ioctl(data->sock, SIOCGIFINDEX, &ifr) == 0)
	{
		this->ent.index = (UInt32)ifr.ifr_ifru.ifru_ivalue;
	}
	else
	{
		this->ent.index = 0;
	}

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
		NEW_CLASS(fs, IO::FileStream(sb.ToCString(), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		readSize = fs->Read(buff, 63);
		DEL_CLASS(fs);
		buff[readSize] = 0;
		this->ent.physicalAddr = 0;
		this->ent.physicalAddrLeng = 0;
		if (readSize > 0)
		{
			UTF8Char *sarr[7];
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
	this->ent.dhcpLeaseTime = 0;
	this->ent.dhcpLeaseExpire = 0;
	this->ent.dnsSuffix = 0;
	
	NEW_CLASS(this->ent.ipaddr, Data::ArrayListUInt32(4));
#if !defined(NO_GETIFADDRS) && (!defined(__ANDROID_API__) || __ANDROID_API__ >= 24)
	struct ifaddrs *ifap;
	struct ifaddrs *ifa;
	if (getifaddrs(&ifap) == 0)
	{
		ifa = ifap;
		while (ifa)
		{
			if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET && Text::StrEquals(ifa->ifa_name, data->name))
			{
				this->ent.ipaddr->Add((UInt32)((sockaddr_in*)ifa->ifa_addr)->sin_addr.s_addr);
			}
			ifa = ifa->ifa_next;
		}
		freeifaddrs(ifap);
	}
#else
#endif
	
	Text::UTF8Reader *reader;
	NEW_CLASS(this->ent.dnsaddr, Data::ArrayListUInt32(4));
	NEW_CLASS(fs, IO::FileStream(CSTR("/etc/resolv.conf"), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (!fs->IsError())
	{
		Text::PString sarr[3];
		NEW_CLASS(reader, Text::UTF8Reader(fs));
		sb.ClearStr();
		while (reader->ReadLine(&sb, 512))
		{
			sb.Trim();
			if (sb.ToString()[0] == '#')
			{
			}
			else if (sb.StartsWith(UTF8STRC("nameserver")))
			{
				if (Text::StrSplitWSP(sarr, 3, sb) >= 2)
				{
					this->ent.dnsaddr->Add(Net::SocketUtil::GetIPAddr(sarr[1].v, sarr[1].leng));
				}
			}
			sb.ClearStr();
		}
		DEL_CLASS(reader);
	}
	DEL_CLASS(fs);
	this->ent.defGW = 0;

/*	NEW_CLASS(fs, IO::FileStream(L"/proc/net/route", IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (!fs->IsError())
	{
		WChar *sarr[4];
		NEW_CLASS(reader, Text::UTF8Reader(fs));
		sb.ClearStr();
		reader->ReadLine(&sb, 512);
		sb.ClearStr();
		while (reader->ReadLine(&sb, 512))
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
