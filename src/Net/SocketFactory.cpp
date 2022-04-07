#include "Stdafx.h"
#include "Data/ByteTool.h"
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

Net::SocketFactory::SocketFactory(Bool noV6DNS)
{
	this->dnsHdlr = 0;
	this->noV6DNS = noV6DNS;
}

Net::SocketFactory::~SocketFactory()
{
	SDEL_CLASS(this->dnsHdlr);
}

Bool Net::SocketFactory::AdapterSetHWAddr(Text::CString adapterName, const UInt8 *hwAddr)
{
	return false;
}

Bool Net::SocketFactory::AdapterEnable(Text::CString adapterName, Bool enable)
{
	return false;
}

Bool Net::SocketFactory::ReloadDNS()
{
	Sync::MutexUsage mutUsage(&this->dnsMut);
	SDEL_CLASS(this->dnsHdlr);
	return true;
}

Bool Net::SocketFactory::DNSResolveIP(const UTF8Char *host, UOSInt hostLen, Net::SocketUtil::AddressInfo *addr)
{
	UTF8Char sbuff[256];

	if (Net::SocketUtil::GetIPAddr(host, hostLen, addr))
		return true;

	UTF8Char *sptr = Text::TextBinEnc::Punycode::Encode(sbuff, Text::CString(host, hostLen));
	Sync::MutexUsage mutUsage(&this->dnsMut);
	if (this->dnsHdlr == 0)
	{
		Net::SocketUtil::AddressInfo dnsAddr;
		this->GetDefDNS(&dnsAddr);
		NEW_CLASS(this->dnsHdlr, Net::DNSHandler(this, &dnsAddr));
		this->LoadHosts(this->dnsHdlr);
	}
	mutUsage.EndUse();
	if (!this->noV6DNS)
	{
		if (this->dnsHdlr->GetByDomainNamev6(addr, sbuff, (UOSInt)(sptr - sbuff)))
			return true;
	}
	Bool succ = this->dnsHdlr->GetByDomainNamev4(addr, sbuff, (UOSInt)(sptr - sbuff));
	if (!succ)
	{
		Net::SocketUtil::AddressInfo dnsAddr;
		this->GetDefDNS(&dnsAddr);
		this->dnsHdlr->UpdateDNSAddr(&dnsAddr);
	}
	return succ;
}

UInt32 Net::SocketFactory::DNSResolveIPv4(const UTF8Char *host, UOSInt hostLen)
{
	Net::SocketUtil::AddressInfo addr;
	UTF8Char sbuff[256];

	if (Net::SocketUtil::GetIPAddr(host, hostLen, &addr))
	{
		return *(UInt32*)addr.addr;
	}

	UTF8Char *sptr = Text::TextBinEnc::Punycode::Encode(sbuff, Text::CString(host, hostLen));
	Sync::MutexUsage mutUsage(&this->dnsMut);
	if (this->dnsHdlr == 0)
	{
		Net::SocketUtil::AddressInfo dnsAddr;
		this->GetDefDNS(&dnsAddr);
		NEW_CLASS(this->dnsHdlr, Net::DNSHandler(this, &dnsAddr));
		this->LoadHosts(this->dnsHdlr);
	}
	mutUsage.EndUse();
	if (this->dnsHdlr->GetByDomainNamev4(&addr, sbuff, (UOSInt)(sptr - sbuff)))
	{
		return *(UInt32*)addr.addr;
	}
	else
	{
		Net::SocketUtil::AddressInfo dnsAddr;
		this->GetDefDNS(&dnsAddr);
		this->dnsHdlr->UpdateDNSAddr(&dnsAddr);
		return 0;
	}
}

UTF8Char *Net::SocketFactory::GetRemoteName(UTF8Char *buff, Socket *socket)
{
	Net::SocketUtil::AddressInfo addr;
	UInt16 port;
	if (GetRemoteAddr(socket, &addr, &port))
	{
		return Net::SocketUtil::GetAddrName(buff, &addr, port);
	}
	else
	{
		*buff = 0;
		return buff;
	}
}

UTF8Char *Net::SocketFactory::GetLocalName(UTF8Char *buff, Socket *socket)
{
	Net::SocketUtil::AddressInfo addr;
	UInt16 port;
	if (GetLocalAddr(socket, &addr, &port))
	{
		return Net::SocketUtil::GetAddrName(buff, &addr, port);
	}
	else
	{
		*buff = 0;
		return buff;
	}
}

UInt64 Net::SocketFactory::GenSocketId(Socket *socket)
{
	Net::SocketUtil::AddressInfo rAddr;
	UInt16 rPort;
	Net::SocketUtil::AddressInfo lAddr;
	UInt16 lPort;
	if (this->GetLocalAddr(socket, &lAddr, &lPort) && this->GetRemoteAddr(socket, &rAddr, &rPort))
		return Net::SocketUtil::CalcCliId(&rAddr) | (((UInt64)rPort) << 32) | (((UInt64)lPort) << 48);
	else
		return 0;
}

void Net::SocketFactory::FromSocketId(UInt64 socketId, UInt32 *ip, UInt16 *port)
{
	if (ip)
	{
		*ip = ReadUInt32((UInt8*)&socketId);
	}
	if (port)
	{
		*port = ReadUInt16(((UInt8*)&socketId) + 4);
	}
}
