#ifndef _SM_NET_WEBSERVER_WEBSERVICEHANDLER
#define _SM_NET_WEBSERVER_WEBSERVICEHANDLER
#include "Data/FastMap.h"
#include "Data/FastStringMapNN.h"
#include "Net/WebServer/HTTPDirectoryHandler.h"

namespace Net
{
	namespace WebServer
	{
		class WebServiceHandler : public Net::WebServer::HTTPDirectoryHandler
		{
		public:
			typedef Bool (CALLBACKFUNC ServiceFunc)(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr);
		private:
			typedef struct
			{
				NN<Text::String> svcPath;
				Data::FastMap<Int32, ServiceFunc> funcs;
			} ServiceInfo;
			
			Data::FastStringMapNN<ServiceInfo> services;

		protected:
			virtual Bool ProcessRequest(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq);
		public:
			WebServiceHandler();
			WebServiceHandler(Text::CStringNN rootDir);
			virtual ~WebServiceHandler();

			void AddService(Text::CStringNN svcPath, Net::WebUtil::RequestMethod reqMeth, ServiceFunc func);
		};
	}
}
#endif
