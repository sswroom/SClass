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
			NN<DB::DBCache> dbCache;
			Bool noLinks;

			void BuildJSON(NN<Text::JSONBuilder> json, NN<DB::DBRow> row);
			void AppendVector(NN<Text::JSONBuilder> json, Text::CStringNN name, NN<Math::Geometry::Vector2D> vec);
		public:
			RESTfulHandler(NN<DB::DBCache> dbCache);
			virtual ~RESTfulHandler();

			virtual Bool ProcessRequest(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq);

			static DB::PageRequest *ParsePageReq(NN<Net::WebServer::IWebRequest> req);
		};
	}
}
#endif
