#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/StringUTF8Map.h"
#include "IO/FileStream.h"
#include "Net/ARPInfo.h"
#include "Net/SocketUtil.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"
#include <net/if.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

typedef struct
{
	UInt32 ifIndex;
	UInt8 addr[6];
	UInt32 ipAddr;
	Net::ARPInfo::ARPType arpType;
} ARPData;


Net::ARPInfo::ARPInfo(void *ipNetRow)
{
	ARPData *data = (ARPData*)ipNetRow;
	this->ifIndex = data->ifIndex;
	this->phyAddr = MemAlloc(UInt8, 6);
	this->phyAddrLen = 6;
	MemCopyNO(this->phyAddr, data->addr, 6);
	this->ipAddr = data->ipAddr;
	this->arpType = data->arpType;
}

Net::ARPInfo::~ARPInfo()
{
	MemFree(this->phyAddr);
}

UInt32 Net::ARPInfo::GetAdaptorIndex()
{
	return this->ifIndex;
}

UOSInt Net::ARPInfo::GetPhysicalAddr(UInt8 *buff)
{
	if (this->phyAddr)
	{
		MemCopyNO(buff, this->phyAddr, this->phyAddrLen);
		return this->phyAddrLen;
	}
	else
	{
		return 0;
	}
}

UInt32 Net::ARPInfo::GetIPAddress()
{
	return this->ipAddr;
}

Net::ARPInfo::ARPType Net::ARPInfo::GetARPType()
{
	return this->arpType;
}

UOSInt Net::ARPInfo::GetARPInfoList(NN<Data::ArrayListNN<Net::ARPInfo>> arpInfoList)
{
	int sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
	if (sock == -1)
		return 0;

	Data::StringUTF8Map<UInt32> indexMap;
	UInt32 ind = 0;
	Char buff[1024];
	ifconf ifc;
	ifreq *ifrend;
	ifreq *ifrcurr;

	ifc.ifc_len = sizeof(buff);
	ifc.ifc_buf = buff;
	if (ioctl(sock, SIOCGIFCONF, &ifc) >= 0)
	{
		ifrcurr = ifc.ifc_req;
		ifrend = ifrcurr + ((UInt32)ifc.ifc_len / sizeof(ifreq));
		while (ifrcurr != ifrend)
		{
			indexMap.Put((const UTF8Char*)ifrcurr->ifr_name, ind);
			ind++;
			ifrcurr++;
		}
	}
	close(sock);

	UOSInt ret = 0;
	Text::PString sarr[7];
	UnsafeArray<UTF8Char> sarr2[7];
	Text::StringBuilderUTF8 sb;
	ARPData data;
	Int32 flags;
	NN<Net::ARPInfo> arp;
	IO::FileStream fs(CSTR("/proc/net/arp"), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (fs.IsError())
	{

	}
	else
	{
		Text::UTF8Reader reader(fs);
		sb.ClearStr();
		if (reader.ReadLine(sb, 1024))
		{
			while (true)
			{
				sb.ClearStr();
				if (!reader.ReadLine(sb, 1024))
				{
					break;
				}

				if (Text::StrSplitWSP(sarr, 7, sb) >= 6)
				{
					data.ifIndex = indexMap.Get(UnsafeArray<const UTF8Char>(sarr[5].v));
					data.ipAddr = Net::SocketUtil::GetIPAddr(sarr[0].ToCString());
					flags = Text::StrToInt32(sarr[2].v);
					data.arpType = (flags & 4)?ARPT_STATIC:ARPT_DYNAMIC;
					if (Text::StrSplit(sarr2, 7, sarr[3].v, ':') == 6)
					{
						data.addr[0] = Text::StrHex2UInt8C(sarr2[0]);
						data.addr[1] = Text::StrHex2UInt8C(sarr2[1]);
						data.addr[2] = Text::StrHex2UInt8C(sarr2[2]);
						data.addr[3] = Text::StrHex2UInt8C(sarr2[3]);
						data.addr[4] = Text::StrHex2UInt8C(sarr2[4]);
						data.addr[5] = Text::StrHex2UInt8C(sarr2[5]);
						NEW_CLASSNN(arp, Net::ARPInfo(&data));
						arpInfoList->Add(arp);
						ret++;
					}
				}
			}
		}
	}
	return ret;
}
