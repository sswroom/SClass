#include "Stdafx.h"
#include "Net/SSHConn.h"
#include "Net/SSHManager.h"
#include "Net/SSHTCPChannel.h"
#include "Text/StringTool.h"

struct Net::SSHConn::ClassData
{
};

Net::SSHConn::SSHConn(NotNullPtr<Net::SocketFactory> sockf, Text::CStringNN host, UInt16 port, Data::Duration timeout)
{
	this->sockf = sockf;
	this->clsData = MemAllocNN(ClassData);
	this->cli = 0;
}

Net::SSHConn::~SSHConn()
{
	MemFreeNN(this->clsData);
}

Bool Net::SSHConn::IsError() const
{
	return true;
}

NotNullPtr<Net::SocketFactory> Net::SSHConn::GetSocketFactory() const
{
	return this->sockf;
}

Optional<Net::TCPClient> Net::SSHConn::GetTCPClient() const
{
	return this->cli;
}

Bool Net::SSHConn::GetHostKeySHA1(UInt8 *buff)
{
	return false;
}

const UTF8Char *Net::SSHConn::GetBanner()
{
	return 0;
}

const UTF8Char *Net::SSHConn::GetActiveAlgorithm(SSHMethodType method)
{
	return 0;
}

Bool Net::SSHConn::GetAuthMethods(Text::CStringNN userName, NotNullPtr<Data::ArrayListStringNN> authMeth)
{
	return false;
}

Bool Net::SSHConn::AuthPassword(Text::CStringNN userName, Text::CStringNN password)
{
	return false;
}

Optional<Net::SSHTCPChannel> Net::SSHConn::RemoteConnect(Socket *sourceSoc, Text::CStringNN remoteHost, UInt16 remotePort)
{
	return 0;
}

Bool Net::SSHConn::ChannelTryRead(SSHChannelHandle *channel, UInt8 *buff, UOSInt maxSize, OutParam<UOSInt> size)
{
	return false;
}

UOSInt Net::SSHConn::ChannelWrite(SSHChannelHandle *channel, const UInt8 *buff, UOSInt size)
{
	return 0;
}

void Net::SSHConn::ChannelClose(SSHChannelHandle *channel)
{
}

void Net::SSHConn::Close()
{
}
