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
			typedef Bool (__stdcall *ServiceFunc)(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, WebServiceHandler *me);
		private:
			typedef struct
			{
				NotNullPtr<Text::String> svcPath;
				Data::FastMap<Int32, ServiceFunc> funcs;
			} ServiceInfo;
			
			Data::FastStringMap<ServiceInfo *> services;

		protected:
			virtual ~WebServiceHandler();
			virtual Bool ProcessRequest(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq);
		public:
			WebServiceHandler();
			WebServiceHandler(Text::CString rootDir);

			void AddService(Text::CStringNN svcPath, Net::WebUtil::RequestMethod reqMeth, ServiceFunc func);
		};
	}
}
#endif
