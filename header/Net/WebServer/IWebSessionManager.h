#ifndef _SM_NET_WEBSERVER_IWEBSESSIONMANAGER
#define _SM_NET_WEBSERVER_IWEBSESSIONMANAGER
#include "AnyType.h"
#include "Net/WebServer/IWebRequest.h"
#include "Net/WebServer/IWebResponse.h"
#include "Net/WebServer/IWebSession.h"

namespace Net
{
	namespace WebServer
	{
		class IWebSessionManager
		{
		public:
			typedef Bool (CALLBACKFUNC SessionHandler)(NN<IWebSession> sess, AnyType userObj);

		protected:
			SessionHandler delHdlr;
			AnyType delHdlrObj;

			IWebSessionManager(SessionHandler delHdlr, AnyType delHdlrObj);
		public:
			virtual ~IWebSessionManager();

			virtual Optional<IWebSession> GetSession(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp) = 0;
			virtual NN<IWebSession> CreateSession(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp) = 0;
			virtual void DeleteSession(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp) = 0;
		};
	}
}
#endif
