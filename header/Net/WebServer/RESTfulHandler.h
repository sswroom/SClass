#ifndef _SM_NET_WEBSERVER_RESTFULHANDLER
#define _SM_NET_WEBSERVER_RESTFULHANDLER
#include "DB/DBCache.h"
#include "Net/WebServer/WebStandardHandler.h"

namespace Net
{
	namespace WebServer
	{
		class RESTfulHandler : public Net::WebServer::WebStandardHandler
		{
		private:
			DB::DBCache *dbCache;

		public:
			RESTfulHandler(DB::DBCache *dbCache);

		protected:
			virtual ~RESTfulHandler();
		public:
			virtual Bool ProcessRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq);
		};
	}
}
#endif
