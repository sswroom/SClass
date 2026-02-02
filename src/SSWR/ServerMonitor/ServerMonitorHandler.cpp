#include "Stdafx.h"
#include "Net/WebServer/HTTPServerUtil.h"
#include "SSWR/ServerMonitor/ServerMonitorHandler.h"
#include "SSWR/ServerMonitor/ServerMonitorSession.h"
#include "Text/JSONBuilder.h"

Bool __stdcall SSWR::ServerMonitor::ServerMonitorHandler::LoginFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr)
{
	NN<ServerMonitorHandler> me = NN<ServerMonitorHandler>::ConvertFrom(svcHdlr);
	ServerMonitorSession sess(me->sessMgr.GetSession(req, resp));
	if (sess.IsValid())
	{
		resp->AddDefHeaders(req);
		me->AddHeaders(req, resp, CSTR("application/json"));
		return Net::WebServer::HTTPServerUtil::SendContent(req, resp, CSTR("application/json"), CSTR("{\"status\":\"ok\"}"));
	}
	req->ParseHTTPForm();
	NN<Text::String> userName;
	NN<Text::String> password;
	if (req->GetHTTPFormStr(CSTR("username")).SetTo(userName) && req->GetHTTPFormStr(CSTR("password")).SetTo(password))
	{
		SSWR::ServerMonitor::ServerMonitorCore::UserRole userRole = SSWR::ServerMonitor::ServerMonitorCore::UserRole::NotLogged;
		if ((userRole = me->core->Login(userName->ToCString(), password->ToCString())) != ServerMonitorCore::UserRole::NotLogged)
		{
			ServerMonitorSession::CreateSession(me->sessMgr, req, resp, userRole);

			resp->AddDefHeaders(req);
			me->AddHeaders(req, resp, CSTR("application/json"));
			return Net::WebServer::HTTPServerUtil::SendContent(req, resp, CSTR("application/json"), CSTR("{\"status\":\"ok\"}"));
		}
	}
	return false;
}

Bool __stdcall SSWR::ServerMonitor::ServerMonitorHandler::GetServersFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr)
{
	NN<ServerMonitorHandler> me = NN<ServerMonitorHandler>::ConvertFrom(svcHdlr);
	ServerMonitorSession sess(me->sessMgr.GetSession(req, resp));
	if (!sess.IsValid())
	{
		resp->AddDefHeaders(req);
		return resp->ResponseError(req, Net::WebStatus::SC_FORBIDDEN);
	}
	Data::ArrayListNN<ServerMonitorCore::ServerInfo> serverList;
	Sync::MutexUsage mutUsage;
	NN<ServerMonitorCore::ServerInfo> serverInfo;
	me->core->GetServerList(serverList, mutUsage);
	Text::JSONBuilder json(Text::JSONBuilder::OT_ARRAY);
	UIntOS i = 0;
	UIntOS j = serverList.GetCount();
	while (i < j)
	{
		serverInfo = serverList.GetItemNoCheck(i);
		json.ArrayBeginObject();
		json.ObjectAddInt32(CSTR("id"), serverInfo->id);
		json.ObjectAddStr(CSTR("name"), serverInfo->name);
		json.ObjectAddStr(CSTR("serverType"), ServerMonitorCore::ServerTypeGetName(serverInfo->serverType));
		json.ObjectAddStr(CSTR("target"), serverInfo->target);
		json.ObjectAddInt32(CSTR("port"), serverInfo->port);
		json.ObjectAddInt32(CSTR("intervalMS"), serverInfo->intervalMS);
		json.ObjectAddInt32(CSTR("timeoutMS"), serverInfo->timeoutMS);
		json.ObjectAddTSStr(CSTR("lastCheck"), serverInfo->lastCheck);
		json.ObjectAddBool(CSTR("lastSuccess"), serverInfo->lastSuccess);
		json.ObjectEnd();
		i++;
	}
	resp->AddDefHeaders(req);
	me->AddHeaders(req, resp, CSTR("application/json"));
	return Net::WebServer::HTTPServerUtil::SendContent(req, resp, CSTR("application/json"), json.Build());
}

Bool __stdcall SSWR::ServerMonitor::ServerMonitorHandler::LogoutFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr)
{
	NN<ServerMonitorHandler> me = NN<ServerMonitorHandler>::ConvertFrom(svcHdlr);
	me->sessMgr.DeleteSession(req, resp);

	resp->AddDefHeaders(req);
	me->AddHeaders(req, resp, CSTR("application/json"));
	return Net::WebServer::HTTPServerUtil::SendContent(req, resp, CSTR("application/json"), CSTR("{\"status\":\"ok\"}"));
}

Bool __stdcall SSWR::ServerMonitor::ServerMonitorHandler::OnSessionCheck(NN<Net::WebServer::WebSession> sess, AnyType userObj)
{
	return false;
}

Bool __stdcall SSWR::ServerMonitor::ServerMonitorHandler::OnSessionDel(NN<Net::WebServer::WebSession> sess, AnyType userObj)
{
	//NN<ServerMonitorHandler> me = userObj.GetNN<ServerMonitorHandler>();
	return ServerMonitorSession::DeleteSession(sess);
}

void SSWR::ServerMonitor::ServerMonitorHandler::AddHeaders(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN contentType)
{
	resp->AddDefHeaders(req);
	resp->AddHeader(CSTR("Content-Type"), contentType);
}

SSWR::ServerMonitor::ServerMonitorHandler::ServerMonitorHandler(NN<ServerMonitorCore> core, Text::CStringNN rootDir) : Net::WebServer::WebServiceHandler(rootDir), sessMgr(CSTR("/api"), OnSessionDel, this, 300, OnSessionCheck, this, CSTR("ServerSession"))
{
	this->core = core;

	this->AddService(CSTR("/api/login"), Net::WebUtil::RequestMethod::HTTP_POST, LoginFunc);
	this->AddService(CSTR("/api/getservers"), Net::WebUtil::RequestMethod::HTTP_GET, GetServersFunc);
	this->AddService(CSTR("/api/logout"), Net::WebUtil::RequestMethod::HTTP_GET, LogoutFunc);
}

SSWR::ServerMonitor::ServerMonitorHandler::~ServerMonitorHandler()
{
}
