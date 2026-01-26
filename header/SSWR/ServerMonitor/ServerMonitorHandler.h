#ifndef _SM_SSWR_SERVERMONITOR_SERVERMONITORHANDLER
#define _SM_SSWR_SERVERMONITOR_SERVERMONITORHANDLER
#include "Net/WebServer/WebServiceHandler.h"

namespace SSWR
{
	namespace ServerMonitor
	{
		class ServerMonitorCore;
		class ServerMonitorHandler : public Net::WebServer::WebServiceHandler
		{
		private:
			NN<ServerMonitorCore> core;

			static Bool __stdcall LoginFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr);
			static Bool __stdcall GetStatusFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr);
			static Bool __stdcall LogoutFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr);
		public:
			ServerMonitorHandler(NN<ServerMonitorCore> core, Text::CStringNN rootDir);
			virtual ~ServerMonitorHandler();
		};
	}
}
#endif