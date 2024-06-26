#include "Stdafx.h"
#include <winsock2.h>
#include "MyMemory.h"
#include "Net/ConnectionInfo.h"
#include "Net/SocketFactory.h"
#include <iphlpapi.h>
#include "Text/MyString.h"
#include "Text/MyStringW.h"

#include <stdio.h>

Net::ConnectionInfo::ConnectionInfo(void *info)
{
	IP_ADAPTER_ADDRESSES *addr = (IP_ADAPTER_ADDRESSES*)info;
	this->ent.internalName = 0;
	this->ent.name = 0;
	this->ent.description = 0;
	this->ent.dnsSuffix = 0;
	this->ent.dhcpSvr = 0;
	if (addr->AdapterName)
	{
		this->ent.internalName = Text::StrCopyNewCh(addr->AdapterName);
	}
	if (addr->FriendlyName)
	{
		this->ent.name = Text::StrToUTF8New(addr->FriendlyName);
	}
	if (addr->Description)
	{
		this->ent.description = Text::StrToUTF8New(addr->Description);
	}
	if (addr->DnsSuffix)
	{
		this->ent.dnsSuffix = Text::StrToUTF8New(addr->DnsSuffix);
	}
	IP_ADAPTER_UNICAST_ADDRESS *ipaddrs = addr->FirstUnicastAddress;
	IP_ADAPTER_DNS_SERVER_ADDRESS *dnsaddrs = addr->FirstDnsServerAddress;

	while (ipaddrs)
	{
		if (ipaddrs->Address.lpSockaddr->sa_family == AF_INET)
		{
			this->ent.ipaddr.Add((UInt32)((sockaddr_in*)ipaddrs->Address.lpSockaddr)->sin_addr.S_un.S_addr);
		}
		ipaddrs = ipaddrs->Next;
	}
	while (dnsaddrs)
	{
		if (dnsaddrs->Address.lpSockaddr->sa_family == AF_INET)
		{
			this->ent.dnsaddr.Add((UInt32)((sockaddr_in*)dnsaddrs->Address.lpSockaddr)->sin_addr.S_un.S_addr);
		}
		dnsaddrs = dnsaddrs->Next;
	}
	this->ent.defGW = 0;
	this->ent.physicalAddrLeng = addr->PhysicalAddressLength;
	this->ent.physicalAddr = MemAlloc(UInt8, this->ent.physicalAddrLeng);
	MemCopyNO(this->ent.physicalAddr, addr->PhysicalAddress, addr->PhysicalAddressLength);
	this->ent.dhcpEnabled = (addr->Flags & IP_ADAPTER_DHCP_ENABLED) != 0;
	this->ent.mtu = addr->Mtu;
	switch (addr->IfType)
	{
	case IF_TYPE_ETHERNET_CSMACD:
		this->ent.connType = Net::ConnectionInfo::ConnectionType::Ethernet;
		break;
	case IF_TYPE_IEEE80211:
		this->ent.connType = Net::ConnectionInfo::ConnectionType::WiFi;
		break;
	case IF_TYPE_SOFTWARE_LOOPBACK:
		this->ent.connType = Net::ConnectionInfo::ConnectionType::Loopback;
		break;
	case IF_TYPE_PPP:
		this->ent.connType = Net::ConnectionInfo::ConnectionType::DialUp;
		break;
#ifdef IF_TYPE_WWANPP
	case IF_TYPE_WWANPP:
		this->ent.connType = Net::ConnectionInfo::ConnectionType::Cellular;
		break;
#else
	case 243:
		this->ent.connType = Net::ConnectionInfo::ConnectionType::Cellular;
		break;
#endif
	default:
//		printf("IfType = %d\r\n", (Int32)addr->IfType);
		this->ent.connType = Net::ConnectionInfo::ConnectionType::Unknown;
		break;
	}
	this->ent.connStatus = (Net::ConnectionInfo::ConnectionStatus)addr->OperStatus;
}

Bool Net::ConnectionInfo::SetInfo(void *info)
{
	IP_ADAPTER_INFO *inf = (IP_ADAPTER_INFO*)info;
	if (!Text::StrEqualsCh(inf->AdapterName, this->ent.internalName.Ptr()))
	{
		return false;
	}
	this->ent.index = inf->Index;
	if (inf->GatewayList.IpAddress.String[0])
	{
		this->ent.defGW = Net::SocketUtil::GetIPAddr(Text::CStringNN::FromPtr((const UTF8Char*)inf->GatewayList.IpAddress.String));
	}
	if (inf->DhcpEnabled != 0)
	{
		this->ent.dhcpSvr = Net::SocketUtil::GetIPAddr(Text::CStringNN::FromPtr((const UTF8Char*)inf->DhcpServer.IpAddress.String));
		this->ent.dhcpLeaseTime = Data::Timestamp::FromEpochSec(inf->LeaseObtained, Data::DateTimeUtil::GetLocalTzQhr());
		this->ent.dhcpLeaseExpire = Data::Timestamp::FromEpochSec(inf->LeaseExpires, Data::DateTimeUtil::GetLocalTzQhr());
	}
	return true;
}

