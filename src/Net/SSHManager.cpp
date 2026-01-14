#include "Stdafx.h"
#include "Net/SSHManager.h"
#include "Sync/Interlocked.h"
#include <libssh2.h>

UInt32 Net::SSHManager::useCnt = 0;

Net::SSHManager::SSHManager(NN<Net::SocketFactory> sockf)
{
	this->sockf = sockf;
	if (Sync::Interlocked::IncrementU32(useCnt) == 1)
	{
		this->error = libssh2_init(0);
	}
	else
	{
		this->error = 0;
	}
}

Net::SSHManager::~SSHManager()
{
	if (Sync::Interlocked::DecrementU32(useCnt) == 0)
	{
		libssh2_exit();
	}
}

Bool Net::SSHManager::IsError() const
{
	return this->error != 0;
}

Int32 Net::SSHManager::GetError() const
{
	return this->error;
}

Optional<Net::SSHClient> Net::SSHManager::CreateClient(Text::CStringNN host, UInt16 port, Text::CStringNN userName, Text::CStringNN password)
{
	if (this->error != 0)
		return nullptr;
	NN<Net::SSHConn> conn;
	NEW_CLASSNN(conn, Net::SSHConn(this->sockf, host, port, 5000));
	if (conn->IsError())
	{
		conn.Delete();
		return nullptr;
	}
	UInt8 hostKey[20];
	if (!conn->GetHostKeySHA1(hostKey))
	{
		conn.Delete();
		return nullptr;
	}
	Data::ArrayListStringNN authMethod;
	if (!conn->GetAuthMethods(userName, authMethod))
	{
		conn.Delete();
		return nullptr;
	}
	if (authMethod.IndexOfC(CSTR("password")) == INVALID_INDEX)
	{
		authMethod.FreeAll();
		conn.Delete();
		return nullptr;
	}
	authMethod.FreeAll();
	if (!conn->AuthPassword(userName, password))
	{
		conn.Delete();
		return nullptr;
	}
	NN<Net::SSHClient> cli;
	NEW_CLASSNN(cli, Net::SSHClient(conn));
	return cli;
}

Optional<Net::SSHConn> Net::SSHManager::CreateConn(Text::CStringNN host, UInt16 port, Data::Duration timeout)
{
	if (this->error != 0)
		return nullptr;
	NN<Net::SSHConn> conn;
	NEW_CLASSNN(conn, Net::SSHConn(this->sockf, host, port, timeout));
	return conn;
}

Text::CStringNN Net::SSHManager::ErrorGetName(Int32 errorCode)
{
	switch (errorCode)
	{
	case 0:
		return CSTR("Success");
	case LIBSSH2_ERROR_SOCKET_NONE:
		return CSTR("Socket None");
	case LIBSSH2_ERROR_BANNER_RECV:
		return CSTR("Banner Recv");
	case LIBSSH2_ERROR_BANNER_SEND:
		return CSTR("Banner Send");
	case LIBSSH2_ERROR_INVALID_MAC:
		return CSTR("Invalid MAC");
	case LIBSSH2_ERROR_KEX_FAILURE:
		return CSTR("Kex Failure");
	case LIBSSH2_ERROR_ALLOC:
		return CSTR("Alloc");
	case LIBSSH2_ERROR_SOCKET_SEND:
		return CSTR("Socket Send");
	case LIBSSH2_ERROR_KEY_EXCHANGE_FAILURE:
		return CSTR("Key Exchange Failure");
	case LIBSSH2_ERROR_TIMEOUT:
		return CSTR("Timeout");
	case LIBSSH2_ERROR_HOSTKEY_INIT:
		return CSTR("Hostkey Init");
	case LIBSSH2_ERROR_HOSTKEY_SIGN:
		return CSTR("Hostkey Sign");
	case LIBSSH2_ERROR_DECRYPT:
		return CSTR("Decrypt");
	case LIBSSH2_ERROR_SOCKET_DISCONNECT:
		return CSTR("Socket Disconnect");
	case LIBSSH2_ERROR_PROTO:
		return CSTR("Proto");
	case LIBSSH2_ERROR_PASSWORD_EXPIRED:
		return CSTR("Password Expired");
	case LIBSSH2_ERROR_FILE:
		return CSTR("File");
	case LIBSSH2_ERROR_METHOD_NONE:
		return CSTR("Method None");
	case LIBSSH2_ERROR_AUTHENTICATION_FAILED:
		return CSTR("Authentication Failed");
	case LIBSSH2_ERROR_PUBLICKEY_UNVERIFIED:
		return CSTR("Publickey Unverified");
	case LIBSSH2_ERROR_CHANNEL_OUTOFORDER:
		return CSTR("Channel OutOfOrder");
	case LIBSSH2_ERROR_CHANNEL_FAILURE:
		return CSTR("Channel Failure");
	case LIBSSH2_ERROR_CHANNEL_REQUEST_DENIED:
		return CSTR("Channel Request Denied");
	case LIBSSH2_ERROR_CHANNEL_UNKNOWN:
		return CSTR("Channel Unknown");
	case LIBSSH2_ERROR_CHANNEL_WINDOW_EXCEEDED:
		return CSTR("Channel Window Exceeded");
	case LIBSSH2_ERROR_CHANNEL_PACKET_EXCEEDED:
		return CSTR("Channel Packet Exceeded");
	case LIBSSH2_ERROR_CHANNEL_CLOSED:
		return CSTR("Channel Closed");
	case LIBSSH2_ERROR_CHANNEL_EOF_SENT:
		return CSTR("Channel EOF Sent");
	case LIBSSH2_ERROR_SCP_PROTOCOL:
		return CSTR("SCP Protocol");
	case LIBSSH2_ERROR_ZLIB:
		return CSTR("Zlib");
	case LIBSSH2_ERROR_SOCKET_TIMEOUT:
		return CSTR("Socket Timeout");
	case LIBSSH2_ERROR_SFTP_PROTOCOL:
		return CSTR("SFTP Protocol");
	case LIBSSH2_ERROR_REQUEST_DENIED:
		return CSTR("Request Denied");
	case LIBSSH2_ERROR_METHOD_NOT_SUPPORTED:
		return CSTR("Method Not Supported");
	case LIBSSH2_ERROR_INVAL:
		return CSTR("Inval");
	case LIBSSH2_ERROR_INVALID_POLL_TYPE:
		return CSTR("Invalid Poll Type");
	case LIBSSH2_ERROR_PUBLICKEY_PROTOCOL:
		return CSTR("Publickey Protocol");
	case LIBSSH2_ERROR_EAGAIN:
		return CSTR("Eagain");
	case LIBSSH2_ERROR_BUFFER_TOO_SMALL:
		return CSTR("Buffer Too Small");
	case LIBSSH2_ERROR_BAD_USE:
		return CSTR("Bad Use");
	case LIBSSH2_ERROR_COMPRESS:
		return CSTR("Compress");
	case LIBSSH2_ERROR_OUT_OF_BOUNDARY:
		return CSTR("Out Of Boundary");
	case LIBSSH2_ERROR_AGENT_PROTOCOL:
		return CSTR("Agent Protocol");
	case LIBSSH2_ERROR_SOCKET_RECV:
		return CSTR("Socket Recv");
	case LIBSSH2_ERROR_ENCRYPT:
		return CSTR("Encrypt");
	case LIBSSH2_ERROR_BAD_SOCKET:
		return CSTR("Bad Socket");
	case -46: //LIBSSH2_ERROR_KNOWN_HOSTS:
		return CSTR("Known Hosts");
	case -47: //LIBSSH2_ERROR_CHANNEL_WINDOW_FULL:
		return CSTR("Channel Window Full");
	case -48: //LIBSSH2_ERROR_KEYFILE_AUTH_FAILED:
		return CSTR("Keyfile Auth Failed");
	case -49: //LIBSSH2_ERROR_RANDGEN:
		return CSTR("Randgen");
	case -50: //LIBSSH2_ERROR_MISSING_USERAUTH_BANNER:
		return CSTR("Missing Userauth Banner");
	case -51: //LIBSSH2_ERROR_ALGO_UNSUPPORTED:
		return CSTR("Algo Unsupported");
	default:
		return CSTR("Unknown");
	}
}
