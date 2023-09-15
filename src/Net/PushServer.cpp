#include "Stdafx.h"
#include "Net/PushServer.h"
#include "Net/PushServerHandler.h"

Net::PushServer::PushServer(NotNullPtr<Net::SocketFactory> sockf, Net::SSLEngine *ssl, UInt16 port, Text::CString fcmKey, IO::LogTool *log) : mgr(sockf, ssl, fcmKey, log)
{
	this->sockf = sockf;
	this->listener = 0;
	NEW_CLASSNN(this->webHdlr, Net::PushServerHandler(this->mgr));
	NEW_CLASS(this->listener, Net::WebServer::WebListener(sockf, 0, this->webHdlr, port, 120, 4, CSTR("PushServer/1.0"), false, Net::WebServer::KeepAlive::Default, true));
	if (this->listener->IsError())
	{
		DEL_CLASS(this->listener);
		this->listener = 0;
	}
	else
	{
		if (log)
			this->listener->SetAccessLog(log, IO::LogHandler::LogLevel::Action);
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
