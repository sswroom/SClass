#ifndef _SM_SSWR_SERVERMONITOR_SERVERMONITORHANDLER
#define _SM_SSWR_SERVERMONITOR_SERVERMONITORHANDLER
#include "Net/WebServer/MemoryWebSessionManager.h"
#include "Net/WebServer/WebServiceHandler.h"
#include "SSWR/ServerMonitor/ServerMonitorData.h"
#include "Text/JSONBuilder.h"

namespace SSWR
{
	namespace ServerMonitor
	{
		class ServerMonitorCore;
		class ServerMonitorHandler : public Net::WebServer::WebServiceHandler
		{
		private:
			NN<ServerMonitorCore> core;
			Net::WebServer::MemoryWebSessionManager sessMgr;


			static Bool __stdcall LoginFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr);
			static Bool __stdcall GetSessionFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr);
			static Bool __stdcall GetServersFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr);
			static Bool __stdcall GetAlertsFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr);
			static Bool __stdcall AddServerURLFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr);
			static Bool __stdcall DeleteServerFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr);
			static Bool __stdcall AddAlertSMTPFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr);
			static Bool __stdcall TestAlertFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr);
			static Bool __stdcall DeleteAlertFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr);
			static Bool __stdcall LogoutFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr);
		
			static Bool __stdcall OnSessionCheck(NN<Net::WebServer::WebSession> sess, AnyType userObj);
			static Bool __stdcall OnSessionDel(NN<Net::WebServer::WebSession> sess, AnyType userObj);
			void AppendServerInfo(NN<Text::JSONBuilder> json, NN<ServerInfo> serverInfo);
			void AppendAlertInfo(NN<Text::JSONBuilder> json, NN<AlertInfo> alertInfo);
		public:
			ServerMonitorHandler(NN<ServerMonitorCore> core, Text::CStringNN rootDir);
			virtual ~ServerMonitorHandler();
		};
	}
}
#endif