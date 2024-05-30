#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/ConnectionInfo.h"
#include "Text/MyString.h"

Net::ConnectionInfo::ConnectionInfo(const Net::ConnectionInfo::ConnectionEntry *ent)
{
	this->ent.index = ent->index;
	UnsafeArray<const Char> nncs;
	if (ent->internalName.SetTo(nncs))
	{
		this->ent.internalName = Text::StrCopyNewCh(nncs);
	}
	else
	{
		this->ent.internalName = 0;
	}
	this->ent.name = Text::StrSCopyNew(ent->name);
	this->ent.description = Text::StrSCopyNew(ent->description);
	this->ent.dnsSuffix = Text::StrSCopyNew(ent->dnsSuffix);
	this->ent.ipaddr.AddAll(ent->ipaddr);
	this->ent.dnsaddr.AddAll(ent->dnsaddr);
	this->ent.defGW = ent->defGW;
	this->ent.dhcpSvr = ent->dhcpSvr;
	this->ent.dhcpLeaseTime = ent->dhcpLeaseTime;
	this->ent.dhcpLeaseExpire = ent->dhcpLeaseExpire;
	if (ent->physicalAddr)
	{
		this->ent.physicalAddrLeng = ent->physicalAddrLeng;
		this->ent.physicalAddr = MemAlloc(UInt8, this->ent.physicalAddrLeng);
		MemCopyNO(this->ent.physicalAddr, ent->physicalAddr, this->ent.physicalAddrLeng);
	}
	else
	{
		this->ent.physicalAddr = 0;
		this->ent.physicalAddrLeng = 0;
	}
	this->ent.mtu = ent->mtu;
	this->ent.dhcpEnabled = ent->dhcpEnabled;
	this->ent.connType = ent->connType;
	this->ent.connStatus = ent->connStatus;
}

Net::ConnectionInfo::~ConnectionInfo()
{
	SDEL_TEXTC(this->ent.internalName);
	SDEL_TEXT(this->ent.name);
	SDEL_TEXT(this->ent.description);
	SDEL_TEXT(this->ent.dnsSuffix);
	if (this->ent.physicalAddr)
	{
		MemFree(this->ent.physicalAddr);
	}
}

UnsafeArrayOpt<UTF8Char> Net::ConnectionInfo::GetName(UnsafeArray<UTF8Char> buff)
{
	UnsafeArray<const UTF8Char> nns;
	if (this->ent.name.SetTo(nns))
		return Text::StrConcat(buff, nns);
	return 0;
}

UnsafeArrayOpt<UTF8Char> Net::ConnectionInfo::GetDescription(UnsafeArray<UTF8Char> buff)
{
	UnsafeArray<const UTF8Char> nns;
	if (this->ent.description.SetTo(nns))
		return Text::StrConcat(buff, nns);
	return 0;
}

UnsafeArrayOpt<UTF8Char> Net::ConnectionInfo::GetDNSSuffix(UnsafeArray<UTF8Char> buff)
{
	UnsafeArray<const UTF8Char> nns;
	if (this->ent.dnsSuffix.SetTo(nns))
		return Text::StrConcat(buff, nns);
	return 0;
}

UInt32 Net::ConnectionInfo::GetIPAddress(UOSInt index)
{
	if (this->ent.ipaddr.GetCount() <= index)
		return 0;
	return (UInt32)this->ent.ipaddr.GetItem(index);
}

UInt32 Net::ConnectionInfo::GetDNSAddress(UOSInt index)
{
	if (this->ent.dnsaddr.GetCount() <= index)
		return 0;
	return (UInt32)this->ent.dnsaddr.GetItem(index);
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

Data::Timestamp Net::ConnectionInfo::GetDhcpLeaseTime()
{
	return this->ent.dhcpLeaseTime;
}

Data::Timestamp Net::ConnectionInfo::GetDhcpLeaseExpire()
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

Text::CStringNN Net::ConnectionInfo::ConnectionTypeGetName(ConnectionType connType)
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

Text::CStringNN Net::ConnectionInfo::ConnectionStatusGetName(ConnectionStatus connStatus)
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
