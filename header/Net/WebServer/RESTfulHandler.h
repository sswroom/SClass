#ifndef _SM_NET_WEBSERVER_RESTFULHANDLER
#define _SM_NET_WEBSERVER_RESTFULHANDLER
#include "DB/DBCache.h"
#include "DB/PageRequest.h"
#include "Net/WebServer/WebStandardHandler.h"
#include "Text/JSONBuilder.h"

namespace Net
{
	namespace WebServer
	{
		class RESTfulHandler : public Net::WebServer::WebStandardHandler
		{
		private:
			DB::DBCache *dbCache;
			Bool noLinks;

			void BuildJSON(Text::JSONBuilder *json, DB::DBRow *row);
			void AppendVector(Text::JSONBuilder *json, Text::CString name, Math::Geometry::Vector2D *vec);
		public:
			RESTfulHandler(DB::DBCache *dbCache);

		protected:
			virtual ~RESTfulHandler();
		public:
			virtual Bool ProcessRequest(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CString subReq);

			static DB::PageRequest *ParsePageReq(NotNullPtr<Net::WebServer::IWebRequest> req);
		};
	}
}
#endif
