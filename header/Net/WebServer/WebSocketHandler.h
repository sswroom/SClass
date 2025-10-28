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
			NN<IO::StreamHandler> stmHdlr;
		public:
			WebSocketHandler(NN<IO::StreamHandler> stmHdlr);
			virtual ~WebSocketHandler();

			virtual Bool ProcessRequest(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq);
		};
	}
}
#endif
