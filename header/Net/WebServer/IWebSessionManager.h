#ifndef _SM_NET_WEBSERVER_IWEBSESSIONMANAGER
#define _SM_NET_WEBSERVER_IWEBSESSIONMANAGER
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
			typedef Bool (__stdcall *SessionHandler)(IWebSession* sess, void *userObj);

		protected:
			SessionHandler delHdlr;
			void *delHdlrObj;

			IWebSessionManager(SessionHandler delHdlr, void *delHdlrObj);
		public:
			virtual ~IWebSessionManager();

			virtual IWebSession *GetSession(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp) = 0;
			virtual IWebSession *CreateSession(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp) = 0;
			virtual void DeleteSession(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp) = 0;
		};
	}
}
#endif
