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
	NEW_CLASS(this->ent.ipaddr, Data::ArrayListUInt32());
	if (ent->ipaddr)
	{
		this->ent.ipaddr->AddAll(ent->ipaddr);
	}
	NEW_CLASS(this->ent.dnsaddr, Data::ArrayListUInt32());
	if (ent->dnsaddr)
	{
		this->ent.dnsaddr->AddAll(ent->dnsaddr);
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

UOSInt Net::ConnectionInfo::GetIndex()
{
	return this->ent.index;
}

UOSInt Net::ConnectionInfo::GetPhysicalAddress(UInt8 *buff, UOSInt buffSize)
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

Text::CString Net::ConnectionInfo::ConnectionTypeGetName(ConnectionType connType)
{
	switch (connType)
	{
	case ConnectionType::Ethernet:
		return CSTR("Ethernet");
	case ConnectionType::WiFi:
		return CSTR("WiFi");
	case ConnectionType::Loopback:
		return CSTR("Loopback");
	case ConnectionType::DialUp:
		return CSTR("Dial-up");
	case ConnectionType::Cellular:
		return CSTR("Cellular");
	case ConnectionType::Bridge:
		return CSTR("Bridge");
	case ConnectionType::Unknown:
	default:
		return CSTR("Unknown");
	}
}

Text::CString Net::ConnectionInfo::ConnectionStatusGetName(ConnectionStatus connStatus)
{
	switch (connStatus)
	{
	case CS_UP:
		return CSTR("Up");
	case CS_DOWN:
		return CSTR("Down");
	case CS_TESTING:
		return CSTR("Testing");
	case CS_DORMANT:
		return CSTR("Dormant");
	case CS_NOTPRESENT:
		return CSTR("Not Present");
	case CS_LOWERLAYERDOWN:
		return CSTR("Low Layer Down");
	case CS_UNKNOWN:
	default:
		return CSTR("Unknown");
	}
}
