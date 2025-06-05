#include "Stdafx.h"
#include "Net/SSHConn.h"
#include "Net/SSHManager.h"
#include "Net/SSHTCPChannel.h"
#include "Sync/MutexUsage.h"
#include "Text/StringTool.h"
#include <libssh2.h>

//#define VERBOSE
#if defined(VERBOSE)
#include <stdio.h>
#endif

struct Net::SSHConn::ClassData
{
	LIBSSH2_SESSION *session;
	Int32 handshakeRet;
	Bool authen;
	Bool blocking;
};

Net::SSHConn::SSHConn(NN<Net::SocketFactory> sockf, Text::CStringNN host, UInt16 port, Data::Duration timeout)
{
	this->sockf = sockf;
	this->clsData = MemAllocNN(ClassData);
	this->clsData->session = libssh2_session_init();
	this->clsData->handshakeRet = 0;
	this->clsData->authen = false;
	this->clsData->blocking = true;
	this->cli = 0;
	this->lastError = 0;
#if defined(VERBOSE)
	printf("SSHConn: session create %s\r\n", this->clsData->session?"succeed":"failed");
#endif
	if (this->clsData->session)
	{
		NN<Net::TCPClient> cli;
		NN<Socket> s;
		NEW_CLASSNN(cli, Net::TCPClient(sockf, host, port, timeout));
		if (cli->IsConnectError() || !cli->GetSocket().SetTo(s))
		{
#if defined(VERBOSE)
			printf("SSHConn: error in connecting to %s:%d\r\n", host.v, port);
#endif
			cli.Delete();
		}
		else
		{
#if defined(VERBOSE)
			printf("SSHConn: Client %s:%d connected\r\n", host.v, port);
#endif
			this->cli = cli;
			this->clsData->handshakeRet = libssh2_session_handshake(this->clsData->session, (libssh2_socket_t)this->sockf->SocketGetFD(s));
#if defined(VERBOSE)
			char *errMsg = "Unknown";
			libssh2_session_last_error(this->clsData->session, &errMsg, 0, 0);
			printf("SSHConn: Client %s:%d handshake result %d (%s) %s\r\n", host.v, port, this->clsData->handshakeRet, Net::SSHManager::ErrorGetName(this->clsData->handshakeRet).v, errMsg);
#endif
		}
	}
}

Net::SSHConn::~SSHConn()
{
	this->Close();
	MemFreeNN(this->clsData);
}

Bool Net::SSHConn::IsError() const
{
	return this->clsData->session == 0 || this->cli.IsNull() || this->clsData->handshakeRet != 0;
}

NN<Net::SocketFactory> Net::SSHConn::GetSocketFactory() const
{
	return this->sockf;
}

Optional<Net::TCPClient> Net::SSHConn::GetTCPClient() const
{
	return this->cli;
}

Bool Net::SSHConn::GetHostKeySHA1(UnsafeArray<UInt8> buff)
{
	Sync::MutexUsage mutUsage(this->mut);
	const Char *fingerprint = libssh2_hostkey_hash(this->clsData->session,  LIBSSH2_HOSTKEY_HASH_SHA1);
	if (fingerprint)
	{
		MemCopyNO(&buff[0], fingerprint, 20);
#if defined(VERBOSE)
		UInt8 sbuff[60];
		Text::StrHexBytes(sbuff, (const UInt8*)fingerprint, 20, ':');
		printf("SSHConn: Host key fingerprint = %s\r\n", sbuff);
#endif
		return true;
	}
	this->lastError = libssh2_session_last_errno(this->clsData->session);
#if defined(VERBOSE)
	printf("SSHConn: Error in getting Host key fingerprint: %d (%s)\r\n", this->lastError, Net::SSHManager::ErrorGetName(this->lastError).v);
#endif
	return false;
}

const UTF8Char *Net::SSHConn::GetBanner()
{
	Sync::MutexUsage mutUsage(this->mut);
	return (const UTF8Char*)libssh2_session_banner_get(this->clsData->session);
}

const UTF8Char *Net::SSHConn::GetActiveAlgorithm(SSHMethodType method)
{
	Sync::MutexUsage mutUsage(this->mut);
	return (const UTF8Char*)libssh2_session_methods(this->clsData->session, (int)method);
}

Bool Net::SSHConn::GetAuthMethods(Text::CStringNN userName, NN<Data::ArrayListStringNN> authMeth)
{
	Sync::MutexUsage mutUsage(this->mut);
	const Char *userauthlist = libssh2_userauth_list(this->clsData->session, (const Char*)userName.v.Ptr(), (unsigned int)userName.leng);
	if (userauthlist)
	{
#if defined(VERBOSE)
		printf("SSHConn: User auth list for %s = %s\r\n", userName.v, userauthlist);
#endif
		Text::StringTool::SplitAsNewString(Text::CStringNN::FromPtr((const UTF8Char*)userauthlist), ',', authMeth);
		return true;
	}
	this->lastError = libssh2_session_last_errno(this->clsData->session);
#if defined(VERBOSE)
	printf("SSHConn: Error in getting Host key fingerprint: %d (%s)\r\n", this->lastError, Net::SSHManager::ErrorGetName(this->lastError).v);
#endif
	return false;
}

Bool Net::SSHConn::AuthPassword(Text::CStringNN userName, Text::CStringNN password)
{
	Sync::MutexUsage mutUsage(this->mut);
	int err = libssh2_userauth_password_ex(this->clsData->session, (const Char*)userName.v.Ptr(), (unsigned int)userName.leng, (const Char*)password.v.Ptr(), (unsigned int)password.leng, 0);
	this->lastError = err;
#if defined(VERBOSE)
	printf("SSHConn: User auth with password: %d (%s)\r\n", err, Net::SSHManager::ErrorGetName(err).v);
#endif
	if (err == 0)
	{
		this->clsData->authen = true;
		return true;
	}
	return false;
}

Optional<Net::SSHTCPChannel> Net::SSHConn::RemoteConnect(Optional<Socket> sourceSoc, Text::CStringNN remoteHost, UInt16 remotePort)
{
	LIBSSH2_CHANNEL *channel;
	Sync::MutexUsage mutUsage(this->mut);
	NN<Socket> nnsourceSoc;
	if (sourceSoc.SetTo(nnsourceSoc))
	{
		UTF8Char sbuff[64];
		Net::SocketUtil::AddressInfo addr;
		UInt16 port;
		if (!this->sockf->GetRemoteAddr(nnsourceSoc, addr, port))
			return 0;
		Net::SocketUtil::GetAddrName(sbuff, addr);
		channel = libssh2_channel_direct_tcpip_ex(this->clsData->session, (const Char*)remoteHost.v.Ptr(), remotePort, (const Char*)sbuff, port);
	}
	else
	{
		channel = libssh2_channel_direct_tcpip_ex(this->clsData->session, (const Char*)remoteHost.v.Ptr(), remotePort, "127.0.0.1", remotePort);
	}
#if defined(VERBOSE)
	if (channel)
		printf("SSHConn: Remote connected to %s:%d\r\n", remoteHost.v, remotePort);
	else
		printf("SSHConn: Remote failed connecting to %s:%d\r\n", remoteHost.v, remotePort);
#endif
	NN<SSHChannelHandle> nnchannel;
	if (!nnchannel.Set((SSHChannelHandle*)channel))
	{
		this->lastError = libssh2_session_last_errno(this->clsData->session);
#if defined(VERBOSE)
	printf("SSHConn: Remote connect error: %d (%s)\r\n", this->lastError, Net::SSHManager::ErrorGetName(this->lastError).v);
#endif
		return 0;
	}
	libssh2_channel_set_blocking(channel, 0);
#if defined(VERBOSE)
	printf("SSHConn: Channel set to non-blocking\r\n");
#endif
/*	if (this->clsData->blocking)
	{
		this->clsData->blocking = false;
	}*/
	NN<Net::SSHTCPChannel> ch;
	NEW_CLASSNN(ch, Net::SSHTCPChannel(*this, nnchannel, remoteHost));
	return ch;
}

Bool Net::SSHConn::ChannelTryRead(NN<SSHChannelHandle>channel, UnsafeArray<UInt8> buff, UOSInt maxSize, OutParam<UOSInt> size)
{
	Sync::MutexUsage mutUsage(this->mut);
	ssize_t sz = libssh2_channel_read_ex((LIBSSH2_CHANNEL*)channel.Ptr(), 0, (Char*)buff.Ptr(), (size_t)maxSize);
	if (sz == LIBSSH2_ERROR_EAGAIN)
		return false;
	if (sz < 0)
	{
#if defined(VERBOSE)
		printf("SSHConn: Channel read error: %d (%s)\r\n", (int)sz, Net::SSHManager::ErrorGetName((Int32)sz).v);
#endif
		this->lastError = (Int32)sz;
		size.Set(0);
		return true;
	}
#if defined(VERBOSE)
	printf("SSHConn: Channel read %d bytes\r\n", (int)sz);
#endif
	size.Set((UOSInt)sz);
	return true;
}

UOSInt Net::SSHConn::ChannelWrite(NN<SSHChannelHandle> channel, UnsafeArray<const UInt8> buff, UOSInt size)
{
	Sync::MutexUsage mutUsage(this->mut);
	ssize_t sz = libssh2_channel_write_ex((LIBSSH2_CHANNEL*)channel.Ptr(), 0, (const Char*)buff.Ptr(), size);
	if (sz >= 0)
	{
#if defined(VERBOSE)
		printf("SSHConn: Channel write %d bytes\r\n", (int)sz);
#endif
		return (UOSInt)sz;
	}
#if defined(VERBOSE)
	printf("SSHConn: Channel write error: %d (%s)\r\n", (int)sz, Net::SSHManager::ErrorGetName((Int32)sz).v);
#endif
	this->lastError = (Int32)sz;
	return 0;
}

void Net::SSHConn::ChannelClose(NN<SSHChannelHandle> channel)
{
	Sync::MutexUsage mutUsage(this->mut);
	libssh2_channel_set_blocking((LIBSSH2_CHANNEL*)channel.Ptr(), -1); 
	int err = libssh2_channel_close((LIBSSH2_CHANNEL*)channel.Ptr());
#if defined(VERBOSE)
	printf("SSHConn: Channel close response: %d (%s)\r\n", err, Net::SSHManager::ErrorGetName(err).v);
#endif
	err = libssh2_channel_free((LIBSSH2_CHANNEL*)channel.Ptr());
#if defined(VERBOSE)
	printf("SSHConn: Channel free response: %d (%s)\r\n", err, Net::SSHManager::ErrorGetName(err).v);
#endif
	this->lastError = err;
}

void Net::SSHConn::Close()
{
	Sync::MutexUsage mutUsage(this->mut);
	if (this->clsData->session)
	{
		libssh2_session_disconnect(this->clsData->session, "Normal Shutdown");
		libssh2_session_free(this->clsData->session);
		this->clsData->session = 0;
	}
	NN<Net::TCPClient> cli;
	if (this->cli.SetTo(cli))
	{
		cli->Close();
		cli.Delete();
		this->cli = 0;
	}
}
