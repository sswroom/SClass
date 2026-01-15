#ifndef _SM_NET_WEBSERVER_SITEROOTHANDLER
#define _SM_NET_WEBSERVER_SITEROOTHANDLER
#include "Net/WebServer/WebStandardHandler.h"

namespace Net
{
	namespace WebServer
	{
		class SiteRootHandler : public Net::WebServer::WebStandardHandler
		{
		private:
			UnsafeArrayOpt<UInt8> faviconBuff;
			UIntOS faviconSize;

		protected:
			Bool DoRequest(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq);
			virtual Bool ProcessRequest(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq);
		public:
			SiteRootHandler(Text::CStringNN faviconPath);
			virtual ~SiteRootHandler();
		};
	}
}
#endif
