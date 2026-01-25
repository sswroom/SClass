#include "Stdafx.h"
#include "SSWR/ServerMonitor/ServerMonitorHandler.h"

Bool __stdcall SSWR::ServerMonitor::ServerMonitorHandler::LoginFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr)
{
	return false;
}

Bool __stdcall SSWR::ServerMonitor::ServerMonitorHandler::GetStatusFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr)
{
	return false;
}

Bool __stdcall SSWR::ServerMonitor::ServerMonitorHandler::LogoutFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr)
{
	return false;
}

SSWR::ServerMonitor::ServerMonitorHandler::ServerMonitorHandler(NN<ServerMonitorCore> core)
{
	this->core = core;

	this->AddService(CSTR("/api/login"), Net::WebUtil::RequestMethod::HTTP_POST, LoginFunc);
	this->AddService(CSTR("/api/getstatus"), Net::WebUtil::RequestMethod::HTTP_GET, GetStatusFunc);
	this->AddService(CSTR("/api/logout"), Net::WebUtil::RequestMethod::HTTP_GET, LogoutFunc);
}

SSWR::ServerMonitor::ServerMonitorHandler::~ServerMonitorHandler()
{
}
