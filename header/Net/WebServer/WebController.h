#ifndef _SM_NET_WEBSERVER_WEBCONTROLLER
#define _SM_NET_WEBSERVER_WEBCONTROLLER
#include "Data/FastMap.h"
#include "Data/FastStringMapNN.h"
#include "Net/WebServer/WebRequest.h"
#include "Net/WebServer/WebResponse.h"

namespace Net
{
	namespace WebServer
	{
		class WebController
		{
		public:
			typedef Bool (CALLBACKFUNC ServiceFunc)(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebController> me);
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

			Bool ProcessRequest(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq);
			void AddService(Text::CStringNN svcPath, Net::WebUtil::RequestMethod reqMeth, ServiceFunc func);
		};
	}
}
#endif
