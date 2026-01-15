#include "Stdafx.h"
#include "Core/ByteTool_C.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/StreamReader.h"
#include "Net/DNSHandler.h"
#include "Net/SocketFactory.h"
#include "Sync/MutexUsage.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"
#include "Text/TextBinEnc/Punycode.h"

Bool Net::SocketFactory::GetEffectiveDNS(NN<Net::SocketUtil::AddressInfo> addr)
{
	NN<Text::String> s;
	if (this->forceDNS.SetTo(s))
	{
		if (Net::SocketUtil::SetAddrInfo(addr, s->ToCString()))
			return true;
	}
	return GetDefDNS(addr);
}

Net::SocketFactory::SocketFactory(Bool noV6DNS)
{
	this->dnsHdlr = nullptr;
	this->noV6DNS = noV6DNS;
	this->forceDNS = nullptr;
}

Net::SocketFactory::~SocketFactory()
{
	this->dnsHdlr.Delete();
	OPTSTR_DEL(this->forceDNS);
}

Bool Net::SocketFactory::AdapterSetHWAddr(Text::CStringNN adapterName, UnsafeArray<const UInt8> hwAddr)
{
	return false;
}

Bool Net::SocketFactory::AdapterEnable(Text::CStringNN adapterName, Bool enable)
{
	return false;
}

Bool Net::SocketFactory::ReloadDNS()
{
	Sync::MutexUsage mutUsage(this->dnsMut);
	this->dnsHdlr.Delete();
	return true;
}

Bool Net::SocketFactory::ForceDNSServer(Text::CStringNN ip)
{
	Net::SocketUtil::AddressInfo dnsAddr;
	if (Net::SocketUtil::SetAddrInfo(dnsAddr, ip))
	{
		Sync::MutexUsage mutUsage(this->dnsMut);
		OPTSTR_DEL(this->forceDNS);
		this->forceDNS = Text::String::New(ip);
		this->dnsHdlr.Delete();
		return true;
	}
	return false;
}

Bool Net::SocketFactory::DNSResolveIP(Text::CStringNN host, NN<Net::SocketUtil::AddressInfo> addr)
{
	UTF8Char sbuff[256];

	if (Net::SocketUtil::SetAddrInfo(addr, host))
		return true;

	UnsafeArray<UTF8Char> sptr = Text::TextBinEnc::Punycode::Encode(sbuff, host);
	NN<Net::DNSHandler> dnsHdlr;
	Sync::MutexUsage mutUsage(this->dnsMut);
	if (!this->dnsHdlr.SetTo(dnsHdlr))
	{
		Net::SocketUtil::AddressInfo dnsAddr;
		this->GetEffectiveDNS(dnsAddr);
		NEW_CLASSNN(dnsHdlr, Net::DNSHandler(*this, dnsAddr, this->log));
		this->dnsHdlr = dnsHdlr;
		this->LoadHosts(dnsHdlr);
	}
	mutUsage.EndUse();
	if (!this->noV6DNS)
	{
		if (dnsHdlr->GetByDomainNamev6(addr, CSTRP(sbuff, sptr)))
			return true;
	}
	Bool succ = dnsHdlr->GetByDomainNamev4(addr, CSTRP(sbuff, sptr));
	if (!succ)
	{
		Net::SocketUtil::AddressInfo dnsAddr;
		this->GetEffectiveDNS(dnsAddr);
		dnsHdlr->UpdateDNSAddr(dnsAddr);
	}
	return succ;
}

UIntOS Net::SocketFactory::DNSResolveIPs(Text::CStringNN host, Data::DataArray<Net::SocketUtil::AddressInfo> addrs)
{
	UTF8Char sbuff[256];

	if (Net::SocketUtil::SetAddrInfo(addrs[0], host))
		return 1;

	UnsafeArray<UTF8Char> sptr = Text::TextBinEnc::Punycode::Encode(sbuff, host);
	NN<Net::DNSHandler> dnsHdlr;
	Sync::MutexUsage mutUsage(this->dnsMut);
	if (!this->dnsHdlr.SetTo(dnsHdlr))
	{
		Net::SocketUtil::AddressInfo dnsAddr;
		this->GetEffectiveDNS(dnsAddr);
		NEW_CLASSNN(dnsHdlr, Net::DNSHandler(*this, dnsAddr, this->log));
		this->dnsHdlr = dnsHdlr;
		this->LoadHosts(dnsHdlr);
	}
	mutUsage.EndUse();
	UIntOS ret = 0;
	if (!this->noV6DNS)
	{
		ret = dnsHdlr->GetByDomainNamesv6(addrs, CSTRP(sbuff, sptr));
	}
	ret += dnsHdlr->GetByDomainNamesv4(addrs.SubArray(ret), CSTRP(sbuff, sptr));
	if (ret == 0)
	{
		Net::SocketUtil::AddressInfo dnsAddr;
		this->GetEffectiveDNS(dnsAddr);
		dnsHdlr->UpdateDNSAddr(dnsAddr);
	}
	return ret;
}

UInt32 Net::SocketFactory::DNSResolveIPv4(Text::CStringNN host)
{
	Net::SocketUtil::AddressInfo addr;
	UTF8Char sbuff[256];

	if (Net::SocketUtil::SetAddrInfo(addr, host))
	{
		return *(UInt32*)addr.addr;
	}

	UnsafeArray<UTF8Char> sptr = Text::TextBinEnc::Punycode::Encode(sbuff, host);
	NN<Net::DNSHandler> dnsHdlr;
	Sync::MutexUsage mutUsage(this->dnsMut);
	if (!this->dnsHdlr.SetTo(dnsHdlr))
	{
		Net::SocketUtil::AddressInfo dnsAddr;
		this->GetEffectiveDNS(dnsAddr);
		NEW_CLASSNN(dnsHdlr, Net::DNSHandler(*this, dnsAddr, this->log));
		this->dnsHdlr = dnsHdlr;
		this->LoadHosts(dnsHdlr);
	}
	mutUsage.EndUse();
	if (dnsHdlr->GetByDomainNamev4(addr, CSTRP(sbuff, sptr)))
	{
		return *(UInt32*)addr.addr;
	}
	else
	{
		Net::SocketUtil::AddressInfo dnsAddr;
		this->GetEffectiveDNS(dnsAddr);
		dnsHdlr->UpdateDNSAddr(dnsAddr);
		return 0;
	}
}

UnsafeArrayOpt<UTF8Char> Net::SocketFactory::GetRemoteName(UnsafeArray<UTF8Char> buff, NN<Socket> socket)
{
	Net::SocketUtil::AddressInfo addr;
	UInt16 port;
	if (GetRemoteAddr(socket, addr, port))
	{
		return Net::SocketUtil::GetAddrName(buff, addr, port);
	}
	else
	{
		*buff = 0;
		return nullptr;
	}
}

UnsafeArrayOpt<UTF8Char> Net::SocketFactory::GetLocalName(UnsafeArray<UTF8Char> buff, NN<Socket> socket)
{
	Net::SocketUtil::AddressInfo addr;
	UInt16 port;
	if (GetLocalAddr(socket, addr, port))
	{
		return Net::SocketUtil::GetAddrName(buff, addr, port);
	}
	else
	{
		*buff = 0;
		return nullptr;
	}
}

UInt64 Net::SocketFactory::GenSocketId(NN<Socket> socket)
{
	Net::SocketUtil::AddressInfo rAddr;
	UInt16 rPort;
	Net::SocketUtil::AddressInfo lAddr;
	UInt16 lPort;
	if (this->GetLocalAddr(socket, lAddr, lPort) && this->GetRemoteAddr(socket, rAddr, rPort))
		return Net::SocketUtil::CalcCliId(rAddr) | (((UInt64)rPort) << 32) | (((UInt64)lPort) << 48);
	else
		return 0;
}

void Net::SocketFactory::FromSocketId(UInt64 socketId, OptOut<UInt32> ip, OptOut<UInt16> port)
{
	ip.Set(ReadUInt32((UInt8*)&socketId));
	port.Set(ReadUInt16(((UInt8*)&socketId) + 4));
}
