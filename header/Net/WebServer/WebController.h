#ifndef _SM_NET_WEBSERVER_WEBCONTROLLER
#define _SM_NET_WEBSERVER_WEBCONTROLLER
#include "Data/FastMap.h"
#include "Data/FastStringMap.h"
#include "Net/WebServer/IWebRequest.h"
#include "Net/WebServer/IWebResponse.h"

namespace Net
{
	namespace WebServer
	{
		class WebController
		{
		public:
			typedef Bool (__stdcall *ServiceFunc)(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, WebController *me);
		private:
			typedef struct
			{
				Text::String *svcPath;
				Data::FastMap<Int32, ServiceFunc> funcs;
			} ServiceInfo;
			
			Data::FastStringMap<ServiceInfo *> services;
			Text::String *svcPath;

		public:
			WebController(Text::CString svcPath);
			virtual ~WebController();

			Bool ProcessRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq);
			void AddService(Text::CString svcPath, Net::WebUtil::RequestMethod reqMeth, ServiceFunc func);
		};
	}
}
#endif
