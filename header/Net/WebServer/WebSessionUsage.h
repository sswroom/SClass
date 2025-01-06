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
			Optional<Net::WebServer::IWebSession> sess;
		public:
			WebSessionUsage();
			WebSessionUsage(Optional<Net::WebServer::IWebSession> sess);
			~WebSessionUsage();

			void Use(Optional<Net::WebServer::IWebSession> sess);
			void EndUse();
			Optional<Net::WebServer::IWebSession> GetSess();
		};
	}
}
#endif
