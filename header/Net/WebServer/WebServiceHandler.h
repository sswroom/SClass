#ifndef _SM_NET_WEBSERVER_WEBSERVICEHANDLER
#define _SM_NET_WEBSERVER_WEBSERVICEHANDLER
#include "Data/Int32Map.h"
#include "Data/FastStringMap.h"
#include "Net/WebServer/WebStandardHandler.h"

namespace Net
{
	namespace WebServer
	{
		class WebServiceHandler : public Net::WebServer::WebStandardHandler
		{
		public:
			typedef Bool (__stdcall *ServiceFunc)(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, WebServiceHandler *me);
		private:
			typedef struct
			{
				Text::String *svcPath;
				Data::Int32Map<ServiceFunc> *funcs;
			} ServiceInfo;
			
			Data::FastStringMap<ServiceInfo *> *services;

		protected:
			virtual ~WebServiceHandler();
			virtual Bool ProcessRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq);
		public:
			WebServiceHandler();

			void AddService(Text::CString svcPath, Net::WebUtil::RequestMethod reqMeth, ServiceFunc func);
		};
	}
}
#endif
