#include "Stdafx.h"
#include "Net/SSHForwarder.h"
#include "Net/SSHTCPChannel.h"

void __stdcall Net::SSHForwarder::OnClientEvent(NotNullPtr<TCPClient> cli, void *userObj, void *cliData, Net::TCPClientMgr::TCPEventType evtType)
{
	if (evtType == Net::TCPClientMgr::TCP_EVENT_DISCONNECT)
	{
		Net::SSHTCPChannel *channel = (Net::SSHTCPChannel*)cliData;
		DEL_CLASS(channel);
		cli.Delete();
	}
}

void __stdcall Net::SSHForwarder::OnClientData(NotNullPtr<TCPClient> cli, void *userObj, void *cliData, const Data::ByteArrayR &buff)
{
	Net::SSHTCPChannel *channel = (Net::SSHTCPChannel*)cliData;
	channel->WriteCont(buff.Ptr(), buff.GetSize());
}

void __stdcall Net::SSHForwarder::OnClientTimeout(NotNullPtr<TCPClient> cli, void *userObj, void *cliData)
{

}

void __stdcall Net::SSHForwarder::OnClientConn(Socket *s, void *userObj)
{
	Net::SSHForwarder *me = (Net::SSHForwarder*)userObj;
	NotNullPtr<Net::SSHTCPChannel> channel;
	if (me->conn->RemoteConnect(s, me->remoteHost->ToCString(), me->remotePort).SetTo(channel))
	{
		NotNullPtr<Net::TCPClient> cli;
		NEW_CLASSNN(cli, Net::TCPClient(me->conn->GetSocketFactory(), s));
		me->cliMgr.AddClient(cli, channel.Ptr());
	}
	else
	{
		me->conn->GetSocketFactory()->DestroySocket(s);
	}
}

Net::SSHForwarder::SSHForwarder(NotNullPtr<Net::SSHConn> conn, UInt16 localPort, Text::CStringNN remoteHost, UInt16 remotePort) : cliMgr(3600, OnClientEvent, OnClientData, this, 4, OnClientTimeout)
{
	this->conn = conn;
	this->remoteHost = Text::String::New(remoteHost);
	this->remotePort = remotePort;
	Net::SocketUtil::AddressInfo addr;
	Net::SocketUtil::SetAddrInfo(addr, CSTR("127.0.0.1"));
	NEW_CLASSNN(this->svr, Net::TCPServer(this->conn->GetSocketFactory(), addr, localPort, log, OnClientConn, this, CSTR(""), true));
}

Net::SSHForwarder::~SSHForwarder()
{
	this->svr.Delete();
	this->remoteHost->Release();
}

Bool Net::SSHForwarder::IsError() const
{
	return this->svr->IsV4Error();
}

UInt16 Net::SSHForwarder::GetListenPort() const
{
	return this->svr->GetListenPort();
}

void Net::SSHForwarder::DoEvents()
{
	void *cliData;
	UInt8 buff[8192];
	UOSInt size;
	Sync::MutexUsage mutUsage;
	Net::TCPClient *cli;
	this->cliMgr.UseGetClient(mutUsage);
	UOSInt i = this->cliMgr.GetClientCount();
	while (i-- > 0)
	{
		if ((cli = this->cliMgr.GetClient(i, &cliData)) != 0)
		{
			if (((Net::SSHTCPChannel*)cliData)->TryRead(buff, sizeof(buff), size))
			{
				if (size == 0)
				{
					cli->Close();
				}
				else
				{
					cli->WriteCont(buff, size);
				}
			}
		}
	}
}

Bool Net::SSHForwarder::HasChannels() const
{
	return this->cliMgr.GetClientCount() > 0;
}