#include "Stdafx.h"
#include "Net/PushServer.h"
#include "Net/PushServerHandler.h"

Net::PushServer::PushServer(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, UInt16 port, NN<Net::Google::GoogleServiceAccount> serviceAccount, NN<IO::LogTool> log) : mgr(clif, ssl, serviceAccount, log)
{
	this->clif = clif;
	this->listener = 0;
	NN<Net::WebServer::WebListener> listener;
	NEW_CLASSNN(this->webHdlr, Net::PushServerHandler(this->mgr));
	NEW_CLASSNN(listener, Net::WebServer::WebListener(clif, 0, this->webHdlr, port, 120, 1, 4, CSTR("PushServer/1.0"), false, Net::WebServer::KeepAlive::Default, true));
	if (listener->IsError())
	{
		listener.Delete();
	}
	else
	{
		this->listener = listener;
		if (log->HasHandler())
			listener->SetAccessLog(log.Ptr(), IO::LogHandler::LogLevel::Action);
	}
}

Net::PushServer::~PushServer()
{
	this->listener.Delete();
	this->webHdlr.Delete();
}

Bool Net::PushServer::IsError()
{
	return this->listener.IsNull();
}
