#ifndef _SM_NET_WEBSERVER_WEBSESSIONMANAGER
#define _SM_NET_WEBSERVER_WEBSESSIONMANAGER
#include "AnyType.h"
#include "Net/WebServer/WebRequest.h"
#include "Net/WebServer/WebResponse.h"
#include "Net/WebServer/WebSession.h"

namespace Net
{
	namespace WebServer
	{
		class WebSessionManager
		{
		public:
			typedef Bool (CALLBACKFUNC SessionHandler)(NN<WebSession> sess, AnyType userObj);

		protected:
			SessionHandler delHdlr;
			AnyType delHdlrObj;

			WebSessionManager(SessionHandler delHdlr, AnyType delHdlrObj);
		public:
			virtual ~WebSessionManager();

			virtual Optional<WebSession> GetSession(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp) = 0;
			virtual NN<WebSession> CreateSession(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp) = 0;
			virtual void DeleteSession(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp) = 0;
		};
	}
}
#endif
