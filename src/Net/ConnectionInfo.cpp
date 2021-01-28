#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/ConnectionInfo.h"
#include "Text/MyString.h"

Net::ConnectionInfo::ConnectionInfo(const Net::ConnectionInfo::ConnectionEntry *ent)
{
	this->ent.index = ent->index;
	if (ent->internalName)
	{
		this->ent.internalName = Text::StrCopyNew(ent->internalName);
	}
	else
	{
		this->ent.internalName = 0;
	}
	if (ent->name)
	{
		this->ent.name = Text::StrCopyNew(ent->name);
	}
	else
	{
		this->ent.name = 0;
	}
	if (ent->description)
	{
		this->ent.description = Text::StrCopyNew(ent->description);
	}
	else
	{
		this->ent.description = 0;
	}
	if (ent->dnsSuffix)
	{
		this->ent.dnsSuffix = Text::StrCopyNew(ent->dnsSuffix);
	}
	else
	{
		this->ent.dnsSuffix = 0;
	}
	NEW_CLASS(this->ent.ipaddr, Data::ArrayListInt32());
	if (ent->ipaddr)
	{
		this->ent.ipaddr->AddRange(ent->ipaddr);
	}
	NEW_CLASS(this->ent.dnsaddr, Data::ArrayListInt32());
	if (ent->dnsaddr)
	{
		this->ent.dnsaddr->AddRange(ent->dnsaddr);
	}
	this->ent.defGW = ent->defGW;
	this->ent.dhcpSvr = ent->dhcpSvr;
	if (ent->dhcpLeaseTime)
	{
		NEW_CLASS(this->ent.dhcpLeaseTime, Data::DateTime(ent->dhcpLeaseTime));
	}
	else
	{
		this->ent.dhcpLeaseTime = 0;
	}
	if (ent->dhcpLeaseExpire)
	{
		NEW_CLASS(this->ent.dhcpLeaseExpire, Data::DateTime(ent->dhcpLeaseExpire));
	}
	else
	{
		this->ent.dhcpLeaseExpire = 0;
	}
	if (ent->physicalAddr)
	{
		this->ent.physicalAddrLeng = ent->physicalAddrLeng;
		this->ent.physicalAddr = MemAlloc(UInt8, this->ent.physicalAddrLeng);
		MemCopyNO(this->ent.physicalAddr, ent->physicalAddr, this->ent.physicalAddrLeng);
	}
	else
	{
		this->ent.physicalAddr = 0;
		this->ent.physicalAddrLeng = ent->physicalAddrLeng;
	}
	this->ent.mtu = ent->mtu;
	this->ent.dhcpEnabled = ent->dhcpEnabled;
	this->ent.connType = ent->connType;
	this->ent.connStatus = ent->connStatus;
}

Net::ConnectionInfo::~ConnectionInfo()
{
	SDEL_TEXT(this->ent.internalName);
	SDEL_TEXT(this->ent.name);
	SDEL_TEXT(this->ent.description);
	SDEL_TEXT(this->ent.dnsSuffix);
	SDEL_CLASS(this->ent.ipaddr);
	SDEL_CLASS(this->ent.dnsaddr);
	SDEL_CLASS(this->ent.dhcpLeaseTime);
	SDEL_CLASS(this->ent.dhcpLeaseExpire);
	if (this->ent.physicalAddr)
	{
		MemFree(this->ent.physicalAddr);
	}
}

UTF8Char *Net::ConnectionInfo::GetName(UTF8Char *buff)
{
	if (this->ent.name)
		return Text::StrConcat(buff, this->ent.name);
	return 0;
}

UTF8Char *Net::ConnectionInfo::GetDescription(UTF8Char *buff)
{
	if (this->ent.description)
		return Text::StrConcat(buff, this->ent.description);
	return 0;
}

UTF8Char *Net::ConnectionInfo::GetDNSSuffix(UTF8Char *buff)
{
	if (this->ent.dnsSuffix)
		return Text::StrConcat(buff, this->ent.dnsSuffix);
	return 0;
}

UInt32 Net::ConnectionInfo::GetIPAddress(UOSInt index)
{
	if (this->ent.ipaddr->GetCount() <= index)
		return 0;
	return (UInt32)this->ent.ipaddr->GetItem(index);
}

UInt32 Net::ConnectionInfo::GetDNSAddress(UOSInt index)
{
	if (this->ent.dnsaddr->GetCount() <= index)
		return 0;
	return (UInt32)this->ent.dnsaddr->GetItem(index);
}

UInt32 Net::ConnectionInfo::GetDefaultGW()
{
	return this->ent.defGW;
}

OSInt Net::ConnectionInfo::GetIndex()
{
	return this->ent.index;
}

OSInt Net::ConnectionInfo::GetPhysicalAddress(UInt8 *buff, OSInt buffSize)
{
	if (buffSize >= this->ent.physicalAddrLeng)
	{
		MemCopyNO(buff, this->ent.physicalAddr, this->ent.physicalAddrLeng);
		return this->ent.physicalAddrLeng;
	}
	else
	{
		return this->ent.physicalAddrLeng;
	}
}

UInt32 Net::ConnectionInfo::GetMTU()
{
	return this->ent.mtu;
}

Bool Net::ConnectionInfo::IsDhcpEnabled()
{
	return this->ent.dhcpEnabled;
}

UInt32 Net::ConnectionInfo::GetDhcpServer()
{
	return this->ent.dhcpSvr;
}
Data::DateTime *Net::ConnectionInfo::GetDhcpLeaseTime()
{
	return this->ent.dhcpLeaseTime;
}

Data::DateTime *Net::ConnectionInfo::GetDhcpLeaseExpire()
{
	return this->ent.dhcpLeaseExpire;
}

Net::ConnectionInfo::ConnectionType Net::ConnectionInfo::GetConnectionType()
{
	return this->ent.connType;
}

Net::ConnectionInfo::ConnectionStatus Net::ConnectionInfo::GetConnectionStatus()
{
	return this->ent.connStatus;
}

const UTF8Char *Net::ConnectionInfo::GetConnectionTypeName(ConnectionType connType)
{
	switch (connType)
	{
	case CT_ETHERNET:
		return (const UTF8Char*)"Ethernet";
	case CT_WIFI:
		return (const UTF8Char*)"WiFi";
	case CT_LOOPBACK:
		return (const UTF8Char*)"Loopback";
	case CT_DIALUP:
		return (const UTF8Char*)"Dial-up";
	default:
		return (const UTF8Char*)"Unknown";
	}
}

const UTF8Char *Net::ConnectionInfo::GetConnectionStatusName(ConnectionStatus connStatus)
{
	switch (connStatus)
	{
	case CS_UP:
		return (const UTF8Char*)"Up";
	case CS_DOWN:
		return (const UTF8Char*)"Down";
	case CS_TESTING:
		return (const UTF8Char*)"Testing";
	case CS_DORMANT:
		return (const UTF8Char*)"Dormant";
	case CS_NOTPRESENT:
		return (const UTF8Char*)"Not Present";
	case CS_LOWERLAYERDOWN:
		return (const UTF8Char*)"Low Layer Down";
	default:
		return (const UTF8Char*)"Unknown";
	}
}
