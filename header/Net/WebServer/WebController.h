#ifndef _SM_NET_WEBSERVER_WEBCONTROLLER
#define _SM_NET_WEBSERVER_WEBCONTROLLER
#include "Data/FastMap.h"
#include "Data/FastStringMapNN.h"
#include "Net/WebServer/IWebRequest.h"
#include "Net/WebServer/IWebResponse.h"

namespace Net
{
	namespace WebServer
	{
		class WebController
		{
		public:
			typedef Bool (__stdcall *ServiceFunc)(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<WebController> me);
		private:
			typedef struct
			{
				NN<Text::String> svcPath;
				Data::FastMap<Int32, ServiceFunc> funcs;
			} ServiceInfo;
			
			Data::FastStringMapNN<ServiceInfo> services;
			NN<Text::String> svcPath;

		public:
			WebController(Text::CStringNN svcPath);
			virtual ~WebController();

			Bool ProcessRequest(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq);
			void AddService(Text::CStringNN svcPath, Net::WebUtil::RequestMethod reqMeth, ServiceFunc func);
		};
	}
}
#endif
