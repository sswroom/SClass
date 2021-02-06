#ifndef _SM_NET_WEBSERVER_WEBSESSIONUSAGE
#define _SM_NET_WEBSERVER_WEBSESSIONUSAGE
#include "Net/WebServer/IWebSession.h"

namespace Net
{
	namespace WebServer
	{
		class WebSessionUsage
		{
		private:
			Net::WebServer::IWebSession *sess;
		public:
			WebSessionUsage();
			WebSessionUsage(Net::WebServer::IWebSession *sess);
			~WebSessionUsage();

			void Use(Net::WebServer::IWebSession *sess);
			void EndUse();
			Net::WebServer::IWebSession *GetSess();
		};
	}
}
#endif
