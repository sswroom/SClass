#include "Stdafx.h"
#include "Net/ProxyServer.h"

void __stdcall Net::ProxyServer::OnClientConn(NN<Socket> s, AnyType userObj)
{
	NN<Net::ProxyServer> me = userObj.GetNN<Net::ProxyServer>();
	NN<Net::TCPClientMgr> cliMgr;
	if (!me->cliMgr.SetTo(cliMgr))
	{
		me->sockf->DestroySocket(s);
		return;
	}
	NN<Net::TCPClient> cli;
	NEW_CLASSNN(cli, Net::TCPClient(me->sockf, s));
	cliMgr->AddClient(cli, 0);
}
		
void __stdcall Net::ProxyServer::OnClientEvent(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData, Net::TCPClientMgr::TCPEventType evtType)
{
//	NN<Net::ProxyServer> me = userObj.GetNN<Net::ProxyServer>();
	if (evtType == Net::TCPClientMgr::TCP_EVENT_DISCONNECT)
	{
		cli.Delete();
	}
}

void __stdcall Net::ProxyServer::OnClientData(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData, const Data::ByteArrayR &buff)
{
//	NN<Net::ProxyServer> me = userObj.GetNN<Net::ProxyServer>();
}

void __stdcall Net::ProxyServer::OnClientTimeout(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData)
{
}

Net::ProxyServer::ProxyServer(NN<Net::SocketFactory> sockf, UInt16 port, NN<IO::LogTool> log, Bool autoStart)
{
	this->sockf = sockf;
	this->log = log;
	this->port = port;
	this->svr = nullptr;
	NEW_CLASSOPT(this->cliMgr, Net::TCPClientMgr(30, OnClientEvent, OnClientData, this, 10, OnClientTimeout));
	NN<Net::TCPServer> svr;
	NEW_CLASSNN(svr, Net::TCPServer(sockf, nullptr, port, log, OnClientConn, this, CSTR("Prx: "), autoStart));
	if (svr->IsV4Error())
	{
		svr.Delete();
		this->cliMgr.Delete();
	}
	else
	{
		this->svr = svr;
	}
}

Net::ProxyServer::~ProxyServer()
{
	this->svr.Delete();
	this->cliMgr.Delete();
}

Bool Net::ProxyServer::Start()
{
	NN<Net::TCPServer> svr;
	return this->svr.SetTo(svr) && svr->Start();
}

Bool Net::ProxyServer::IsError()
{
	return this->svr.IsNull();
}
