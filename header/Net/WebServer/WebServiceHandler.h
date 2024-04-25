#ifndef _SM_NET_WEBSERVER_WEBSERVICEHANDLER
#define _SM_NET_WEBSERVER_WEBSERVICEHANDLER
#include "Data/FastMap.h"
#include "Data/FastStringMap.h"
#include "Net/WebServer/HTTPDirectoryHandler.h"

namespace Net
{
	namespace WebServer
	{
		class WebServiceHandler : public Net::WebServer::HTTPDirectoryHandler
		{
		public:
			typedef Bool (__stdcall *ServiceFunc)(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr);
		private:
			typedef struct
			{
				NN<Text::String> svcPath;
				Data::FastMap<Int32, ServiceFunc> funcs;
			} ServiceInfo;
			
			Data::FastStringMap<ServiceInfo *> services;

		protected:
			virtual Bool ProcessRequest(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq);
		public:
			WebServiceHandler();
			WebServiceHandler(Text::CStringNN rootDir);
			virtual ~WebServiceHandler();

			void AddService(Text::CStringNN svcPath, Net::WebUtil::RequestMethod reqMeth, ServiceFunc func);
		};
	}
}
#endif
