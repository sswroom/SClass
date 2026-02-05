#include "Stdafx.h"
#include "Net/WebServer/HTTPServerUtil.h"
#include "SSWR/ServerMonitor/ServerMonitorCore.h"
#include "SSWR/ServerMonitor/ServerMonitorHandler.h"
#include "SSWR/ServerMonitor/ServerMonitorSession.h"
#include "Text/JSONBuilder.h"

Bool __stdcall SSWR::ServerMonitor::ServerMonitorHandler::LoginFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr)
{
	NN<ServerMonitorHandler> me = NN<ServerMonitorHandler>::ConvertFrom(svcHdlr);
	ServerMonitorSession sess(me->sessMgr.GetSession(req, resp));
	if (sess.IsValid())
	{
		return me->ResponseJSONStr(req, resp, 0, CSTR("{\"status\":\"ok\"}"));
	}
	req->ParseHTTPForm();
	NN<Text::String> userName;
	NN<Text::String> password;
	if (req->GetHTTPFormStr(CSTR("username")).SetTo(userName) && req->GetHTTPFormStr(CSTR("password")).SetTo(password))
	{
		SSWR::ServerMonitor::UserRole userRole = SSWR::ServerMonitor::UserRole::NotLogged;
		if ((userRole = me->core->Login(userName->ToCString(), password->ToCString())) != UserRole::NotLogged)
		{
			ServerMonitorSession::CreateSession(me->sessMgr, req, resp, userRole);
			return me->ResponseJSONStr(req, resp, 0, CSTR("{\"status\":\"ok\"}"));
		}
	}
	return false;
}

Bool __stdcall SSWR::ServerMonitor::ServerMonitorHandler::GetSessionFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr)
{
	NN<ServerMonitorHandler> me = NN<ServerMonitorHandler>::ConvertFrom(svcHdlr);
	ServerMonitorSession sess(me->sessMgr.GetSession(req, resp));
	UserRole userRole;
	if (!sess.IsValid())
	{
		userRole = UserRole::NotLogged;
	}
	else
	{
		userRole = sess.GetUserRole();
	}
	Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
	json.ObjectAddInt32(CSTR("userRole"), (Int32)userRole);
	return me->ResponseJSONStr(req, resp, 0, json.Build());
}

Bool __stdcall SSWR::ServerMonitor::ServerMonitorHandler::GetServersFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr)
{
	NN<ServerMonitorHandler> me = NN<ServerMonitorHandler>::ConvertFrom(svcHdlr);
	ServerMonitorSession sess(me->sessMgr.GetSession(req, resp));
	if (!sess.IsValid())
	{
		return me->ResponseStatus(req, resp, 0, Net::WebStatus::SC_FORBIDDEN);
	}
	Data::ArrayListNN<ServerInfo> serverList;
	Sync::MutexUsage mutUsage;
	NN<ServerInfo> serverInfo;
	me->core->GetServerList(serverList, mutUsage);
	Text::JSONBuilder json(Text::JSONBuilder::OT_ARRAY);
	UIntOS i = 0;
	UIntOS j = serverList.GetCount();
	while (i < j)
	{
		serverInfo = serverList.GetItemNoCheck(i);
		json.ArrayBeginObject();
		me->AppendServerInfo(json, serverInfo);
		json.ObjectEnd();
		i++;
	}
	return me->ResponseJSONStr(req, resp, 0, json.Build());
}

Bool __stdcall SSWR::ServerMonitor::ServerMonitorHandler::GetAlertsFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr)
{
	NN<ServerMonitorHandler> me = NN<ServerMonitorHandler>::ConvertFrom(svcHdlr);
	ServerMonitorSession sess(me->sessMgr.GetSession(req, resp));
	if (!sess.IsValid())
	{
		return me->ResponseStatus(req, resp, 0, Net::WebStatus::SC_FORBIDDEN);
	}
	Data::ArrayListNN<AlertInfo> alertList;
	Sync::MutexUsage mutUsage;
	NN<AlertInfo> alertInfo;
	me->core->GetAlertList(alertList, mutUsage);
	Text::JSONBuilder json(Text::JSONBuilder::OT_ARRAY);
	UIntOS i = 0;
	UIntOS j = alertList.GetCount();
	while (i < j)
	{
		alertInfo = alertList.GetItemNoCheck(i);
		json.ArrayBeginObject();
		me->AppendAlertInfo(json, alertInfo);
		json.ObjectEnd();
		i++;
	}
	return me->ResponseJSONStr(req, resp, 0, json.Build());
}

Bool __stdcall SSWR::ServerMonitor::ServerMonitorHandler::AddServerURLFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr)
{
	NN<ServerMonitorHandler> me = NN<ServerMonitorHandler>::ConvertFrom(svcHdlr);
	ServerMonitorSession sess(me->sessMgr.GetSession(req, resp));
	if (!sess.IsValid())
	{
		return me->ResponseStatus(req, resp, 0, Net::WebStatus::SC_FORBIDDEN);
	}
	NN<Text::String> name;
	NN<Text::String> url;
	Optional<Text::String> containsText;
	NN<Text::String> s;
	Int32 timeoutMS = 5000;
	req->ParseHTTPForm();
	if (!req->GetHTTPFormStr(CSTR("name")).SetTo(name) || !req->GetHTTPFormStr(CSTR("url")).SetTo(url) || !req->GetHTTPFormInt32(CSTR("timeoutMS"), timeoutMS) || timeoutMS <= 0 || !req->GetHTTPFormStr(CSTR("containsText")).SetTo(s))
	{
		if (req->GetHTTPFormStr(CSTR("name")).IsNull())
		{
			printf("ServerMonitorHandler.AddServerURLFunc: name missing\r\n");
		}
		if (req->GetHTTPFormStr(CSTR("url")).IsNull())
		{
			printf("ServerMonitorHandler.AddServerURLFunc: url missing\r\n");
		}
		if (timeoutMS <= 0)
		{
			printf("ServerMonitorHandler.AddServerURLFunc: invalid timeoutMS\r\n");
		}
		if (req->GetHTTPFormStr(CSTR("containsText")).IsNull())
		{
			printf("ServerMonitorHandler.AddServerURLFunc: containsText missing\r\n");
		}
		return me->ResponseStatus(req, resp, 0, Net::WebStatus::SC_BAD_REQUEST);
	}
	if (!url->StartsWithICase(CSTR("http://")) && !url->StartsWithICase(CSTR("https://")))
	{
		return me->ResponseStatus(req, resp, 0, Net::WebStatus::SC_BAD_REQUEST);
	}
	if (s->leng > 0)
	{
		containsText = s;
	}
	else
	{
		containsText = nullptr;
	}
	NN<ServerInfo> serverInfo;
	if (!me->core->AddServerURL(name->ToCString(), url->ToCString(), OPTSTR_CSTR(containsText), timeoutMS).SetTo(serverInfo))
	{
		return me->ResponseStatus(req, resp, 0, Net::WebStatus::SC_INTERNAL_SERVER_ERROR);
	}
	Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
	me->AppendServerInfo(json, serverInfo);
	return me->ResponseJSONStr(req, resp, 0, json.Build());
}

Bool __stdcall SSWR::ServerMonitor::ServerMonitorHandler::LogoutFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr)
{
	NN<ServerMonitorHandler> me = NN<ServerMonitorHandler>::ConvertFrom(svcHdlr);
	me->sessMgr.DeleteSession(req, resp);

	return me->ResponseJSONStr(req, resp, 0, CSTR("{\"status\":\"ok\"}"));
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

void SSWR::ServerMonitor::ServerMonitorHandler::AppendServerInfo(NN<Text::JSONBuilder> json, NN<ServerInfo> serverInfo)
{
	json->ObjectAddInt32(CSTR("id"), serverInfo->id);
	json->ObjectAddStr(CSTR("name"), serverInfo->name);
	json->ObjectAddStr(CSTR("serverType"), ServerTypeGetName(serverInfo->serverType));
	json->ObjectAddStr(CSTR("target"), serverInfo->target);
	json->ObjectAddInt32(CSTR("intervalMS"), serverInfo->intervalMS);
	json->ObjectAddInt32(CSTR("timeoutMS"), serverInfo->timeoutMS);
	json->ObjectAddTSStr(CSTR("lastCheck"), serverInfo->lastCheck);
	json->ObjectAddBool(CSTR("lastSuccess"), serverInfo->lastSuccess);
}

void SSWR::ServerMonitor::ServerMonitorHandler::AppendAlertInfo(NN<Text::JSONBuilder> json, NN<AlertInfo> alertInfo)
{
	json->ObjectAddInt32(CSTR("id"), alertInfo->id);
	json->ObjectAddStr(CSTR("type"), AlertTypeGetName(alertInfo->type));
	json->ObjectAddStr(CSTR("settings"), alertInfo->settings);
	json->ObjectAddStr(CSTR("targets"), alertInfo->targets);
}

SSWR::ServerMonitor::ServerMonitorHandler::ServerMonitorHandler(NN<ServerMonitorCore> core, Text::CStringNN rootDir) : Net::WebServer::WebServiceHandler(rootDir), sessMgr(CSTR("/api"), OnSessionDel, this, 300, OnSessionCheck, this, CSTR("ServerSession"))
{
	this->core = core;

	this->AddService(CSTR("/api/login"), Net::WebUtil::RequestMethod::HTTP_POST, LoginFunc);
	this->AddService(CSTR("/api/session"), Net::WebUtil::RequestMethod::HTTP_GET, GetSessionFunc);
	this->AddService(CSTR("/api/server"), Net::WebUtil::RequestMethod::HTTP_GET, GetServersFunc);
	this->AddService(CSTR("/api/alert"), Net::WebUtil::RequestMethod::HTTP_GET, GetAlertsFunc);
	this->AddService(CSTR("/api/server/url"), Net::WebUtil::RequestMethod::HTTP_PUT, AddServerURLFunc);
	this->AddService(CSTR("/api/logout"), Net::WebUtil::RequestMethod::HTTP_GET, LogoutFunc);
}

SSWR::ServerMonitor::ServerMonitorHandler::~ServerMonitorHandler()
{
}
