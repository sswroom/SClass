#ifndef _SM_NET_WEBSERVER_WEBSOCKETHANDLER
#define _SM_NET_WEBSERVER_WEBSOCKETHANDLER
#include "IO/StreamHandler.h"
#include "Net/WebServer/WebStandardHandler.h"

namespace Net
{
	namespace WebServer
	{
		class WebSocketHandler : public Net::WebServer::WebStandardHandler
		{
		private:
			IO::StreamHandler *stmHdlr;
		public:
			WebSocketHandler(IO::StreamHandler *stmHdlr);
			virtual ~WebSocketHandler();

			virtual Bool ProcessRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq);
		};
	}
}
#endif