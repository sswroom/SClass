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
			UInt8 *faviconBuff;
			UOSInt faviconSize;

		protected:
			virtual ~SiteRootHandler();
			virtual Bool ProcessRequest(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CString subReq);
		public:
			SiteRootHandler(Text::CStringNN faviconPath);
		};
	}
}
#endif
