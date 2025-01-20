#ifndef _SM_NET_WEBSERVER_WEBSESSIONUSAGE
#define _SM_NET_WEBSERVER_WEBSESSIONUSAGE
#include "Net/WebServer/WebSession.h"

namespace Net
{
	namespace WebServer
	{
		class WebSessionUsage
		{
		private:
			Optional<Net::WebServer::WebSession> sess;
		public:
			WebSessionUsage();
			WebSessionUsage(Optional<Net::WebServer::WebSession> sess);
			~WebSessionUsage();

			void Use(Optional<Net::WebServer::WebSession> sess);
			void EndUse();
			Optional<Net::WebServer::WebSession> GetSess();
		};
	}
}
#endif
