#include "Stdafx.h"
#include "Net/ProxyServer.h"

void __stdcall Net::ProxyServer::OnClientConn(Socket *s, AnyType userObj)
{
	NotNullPtr<Net::ProxyServer> me = userObj.GetNN<Net::ProxyServer>();
	NotNullPtr<Net::TCPClient> cli;
	NEW_CLASSNN(cli, Net::TCPClient(me->sockf, s));
	me->cliMgr->AddClient(cli, 0);
}
		
void __stdcall Net::ProxyServer::OnClientEvent(NotNullPtr<Net::TCPClient> cli, AnyType userObj, AnyType cliData, Net::TCPClientMgr::TCPEventType evtType)
{
//	NotNullPtr<Net::ProxyServer> me = userObj.GetNN<Net::ProxyServer>();
	if (evtType == Net::TCPClientMgr::TCP_EVENT_DISCONNECT)
	{
		cli.Delete();
	}
}

void __stdcall Net::ProxyServer::OnClientData(NotNullPtr<Net::TCPClient> cli, AnyType userObj, AnyType cliData, const Data::ByteArrayR &buff)
{
//	NotNullPtr<Net::ProxyServer> me = userObj.GetNN<Net::ProxyServer>();
}

void __stdcall Net::ProxyServer::OnClientTimeout(NotNullPtr<Net::TCPClient> cli, AnyType userObj, AnyType cliData)
{
}

Net::ProxyServer::ProxyServer(NotNullPtr<Net::SocketFactory> sockf, UInt16 port, NotNullPtr<IO::LogTool> log, Bool autoStart)
{
	this->sockf = sockf;
	this->log = log;
	this->port = port;
	this->svr = 0;
	this->cliMgr = 0;
	NEW_CLASS(this->cliMgr, Net::TCPClientMgr(30, OnClientEvent, OnClientData, this, 10, OnClientTimeout));
	NEW_CLASS(this->svr, Net::TCPServer(sockf, 0, port, log, OnClientConn, this, CSTR("Prx: "), autoStart));
	if (this->svr->IsV4Error())
	{
		DEL_CLASS(this->svr);
		DEL_CLASS(this->cliMgr);
		this->svr = 0;
		this->cliMgr = 0;
	}
}

Net::ProxyServer::~ProxyServer()
{
	SDEL_CLASS(this->svr);
	SDEL_CLASS(this->cliMgr);
}

Bool Net::ProxyServer::Start()
{
	return this->svr != 0 && this->svr->Start();
}

Bool Net::ProxyServer::IsError()
{
	return this->svr == 0;
}
