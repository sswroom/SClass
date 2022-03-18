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
	this->ent.ipaddr = 0;
	this->ent.dnsSuffix = 0;
	this->ent.dhcpSvr = 0;
	this->ent.dhcpLeaseTime = 0;
	this->ent.dhcpLeaseExpire = 0;
	if (addr->AdapterName)
	{
		this->ent.internalName = Text::StrCopyNew(addr->AdapterName);
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
	NEW_CLASS(this->ent.ipaddr, Data::ArrayListUInt32(4));
	NEW_CLASS(this->ent.dnsaddr, Data::ArrayListUInt32(4));
	IP_ADAPTER_UNICAST_ADDRESS *ipaddrs = addr->FirstUnicastAddress;
	IP_ADAPTER_DNS_SERVER_ADDRESS *dnsaddrs = addr->FirstDnsServerAddress;

	while (ipaddrs)
	{
		if (ipaddrs->Address.lpSockaddr->sa_family == AF_INET)
		{
			this->ent.ipaddr->Add((UInt32)((sockaddr_in*)ipaddrs->Address.lpSockaddr)->sin_addr.S_un.S_addr);
		}
		ipaddrs = ipaddrs->Next;
	}
	while (dnsaddrs)
	{
		if (dnsaddrs->Address.lpSockaddr->sa_family == AF_INET)
		{
			this->ent.dnsaddr->Add((UInt32)((sockaddr_in*)dnsaddrs->Address.lpSockaddr)->sin_addr.S_un.S_addr);
		}
		dnsaddrs = dnsaddrs->Next;
	}
	this->ent.defGW = 0;
	this->ent.physicalAddr = MemAlloc(UInt8, this->ent.physicalAddrLeng = addr->PhysicalAddressLength);
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
	case IF_TYPE_WWANPP:
		this->ent.connType = Net::ConnectionInfo::ConnectionType::Cellular;
		break;
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
	if (!Text::StrEquals(inf->AdapterName, this->ent.internalName))
	{
		return false;
	}
	this->ent.index = inf->Index;
	if (inf->GatewayList.IpAddress.String[0])
	{
		this->ent.defGW = Net::SocketUtil::GetIPAddr((const UTF8Char*)inf->GatewayList.IpAddress.String, Text::StrCharCnt(inf->GatewayList.IpAddress.String));
	}
	if (inf->DhcpEnabled != 0)
	{
		this->ent.dhcpSvr = Net::SocketUtil::GetIPAddr((const UTF8Char*)inf->DhcpServer.IpAddress.String, Text::StrCharCnt(inf->GatewayList.IpAddress.String));
		if (this->ent.dhcpLeaseTime == 0)
		{
			NEW_CLASS(this->ent.dhcpLeaseTime, Data::DateTime());
		}
		if (this->ent.dhcpLeaseExpire == 0)
		{
			NEW_CLASS(this->ent.dhcpLeaseExpire, Data::DateTime());
		}
		this->ent.dhcpLeaseTime->SetUnixTimestamp(inf->LeaseObtained);
		this->ent.dhcpLeaseTime->ToLocalTime();
		this->ent.dhcpLeaseExpire->SetUnixTimestamp(inf->LeaseExpires);
		this->ent.dhcpLeaseExpire->ToLocalTime();
	}
	return true;
}

