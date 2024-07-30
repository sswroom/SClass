#include "Stdafx.h"
#include "Net/PushServer.h"
#include "Net/PushServerHandler.h"

Net::PushServer::PushServer(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, UInt16 port, Text::CStringNN fcmKey, NN<IO::LogTool> log) : mgr(clif, ssl, fcmKey, log)
{
	this->clif = clif;
	this->listener = 0;
	NEW_CLASSNN(this->webHdlr, Net::PushServerHandler(this->mgr));
	NEW_CLASS(this->listener, Net::WebServer::WebListener(clif, 0, this->webHdlr, port, 120, 1, 4, CSTR("PushServer/1.0"), false, Net::WebServer::KeepAlive::Default, true));
	if (this->listener->IsError())
	{
		DEL_CLASS(this->listener);
		this->listener = 0;
	}
	else
	{
		if (log->HasHandler())
			this->listener->SetAccessLog(log.Ptr(), IO::LogHandler::LogLevel::Action);
	}
}

Net::PushServer::~PushServer()
{
	SDEL_CLASS(this->listener);
	this->webHdlr.Delete();
}

Bool Net::PushServer::IsError()
{
	return this->listener == 0;
}
