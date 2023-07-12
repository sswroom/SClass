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
			typedef Bool (__stdcall *ServiceFunc)(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, WebServiceHandler *me);
		private:
			typedef struct
			{
				NotNullPtr<Text::String> svcPath;
				Data::FastMap<Int32, ServiceFunc> funcs;
			} ServiceInfo;
			
			Data::FastStringMap<ServiceInfo *> services;

		protected:
			virtual ~WebServiceHandler();
			virtual Bool ProcessRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq);
		public:
			WebServiceHandler();
			WebServiceHandler(Text::CString rootDir);

			void AddService(Text::CString svcPath, Net::WebUtil::RequestMethod reqMeth, ServiceFunc func);
		};
	}
}
#endif
