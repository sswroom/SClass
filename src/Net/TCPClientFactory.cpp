#include "Stdafx.h"
#include "Net/HTTPProxyTCPClient.h"
#include "Net/TCPClientFactory.h"

Net::TCPClientFactory::TCPClientFactory(NN<Net::SocketFactory> sockf)
{
	this->sockf = sockf;
	this->proxyHost = nullptr;
	this->proxyPort = 0;
	this->proxyUser = nullptr;
	this->proxyPwd = nullptr;
}

Net::TCPClientFactory::~TCPClientFactory()
{
}

void Net::TCPClientFactory::SetProxy(Text::CStringNN proxyHost, UInt16 proxyPort, Text::CString proxyUser, Text::CString proxyPwd)
{
	this->proxyHost = proxyHost;
	this->proxyPort = proxyPort;
	this->proxyUser = proxyUser;
	this->proxyPwd = proxyPwd;
}

NN<Net::SocketFactory> Net::TCPClientFactory::GetSocketFactory() const
{
	return this->sockf;
}

Text::CString Net::TCPClientFactory::GetProxyHost() const
{
	return this->proxyHost;
}

UInt16 Net::TCPClientFactory::GetProxyPort() const
{
	return this->proxyPort;
}

Text::CString Net::TCPClientFactory::GetProxyUser() const
{
	return this->proxyUser;
}

Text::CString Net::TCPClientFactory::GetProxyPwd() const
{
	return this->proxyPwd;
}

NN<Net::TCPClient> Net::TCPClientFactory::Create(Text::CStringNN name, UInt16 port, Data::Duration timeout)
{
	NN<TCPClient> cli;
	Text::CStringNN proxyHost;
	if (this->proxyHost.SetTo(proxyHost) && proxyPort)
	{
		NEW_CLASSNN(cli, Net::HTTPProxyTCPClient(this->sockf, proxyHost, proxyPort, (this->proxyUser.IsNull() || this->proxyPwd.IsNull())?Net::HTTPProxyTCPClient::PWDT_NONE:Net::HTTPProxyTCPClient::PWDT_BASIC, this->proxyUser.v, this->proxyPwd.v, name, port))
	}
	else
	{
		NEW_CLASSNN(cli, Net::TCPClient(this->sockf, name, port, timeout));
	}
	return cli;
}

NN<Net::TCPClient> Net::TCPClientFactory::Create(UInt32 ip, UInt16 port, Data::Duration timeout)
{
	NN<TCPClient> cli;
	Text::CStringNN proxyHost;
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	if (this->proxyHost.SetTo(proxyHost) && proxyPort)
	{
		sptr = Net::SocketUtil::GetIPv4Name(sbuff, ip);
		NEW_CLASSNN(cli, Net::HTTPProxyTCPClient(this->sockf, proxyHost, proxyPort, (this->proxyUser.IsNull() || this->proxyPwd.IsNull())?Net::HTTPProxyTCPClient::PWDT_NONE:Net::HTTPProxyTCPClient::PWDT_BASIC, this->proxyUser.v, this->proxyPwd.v, CSTRP(sbuff, sptr), port))
	}
	else
	{
		NEW_CLASSNN(cli, Net::TCPClient(this->sockf, ip, port, timeout));
	}
	return cli;
}

NN<Net::TCPClient> Net::TCPClientFactory::Create(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::Duration timeout)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	NN<TCPClient> cli;
	Text::CStringNN proxyHost;
	if (this->proxyHost.SetTo(proxyHost) && proxyPort)
	{
		sptr = Net::SocketUtil::GetAddrName(sbuff, addr).Or(sbuff);
		NEW_CLASSNN(cli, Net::HTTPProxyTCPClient(this->sockf, proxyHost, proxyPort, (this->proxyUser.IsNull() || this->proxyPwd.IsNull())?Net::HTTPProxyTCPClient::PWDT_NONE:Net::HTTPProxyTCPClient::PWDT_BASIC, this->proxyUser.v, this->proxyPwd.v, CSTRP(sbuff, sptr), port))
	}
	else
	{
		NEW_CLASSNN(cli, Net::TCPClient(this->sockf, addr, port, timeout));
	}
	return cli;
}
