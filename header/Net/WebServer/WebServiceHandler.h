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
			typedef Bool (__stdcall *ServiceFunc)(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, WebServiceHandler *me);
		private:
			typedef struct
			{
				Text::String *svcPath;
				Data::Int32Map<ServiceFunc> *funcs;
			} ServiceInfo;
			
			Data::FastStringMap<ServiceInfo *> *services;

		protected:
			virtual ~WebServiceHandler();
			virtual Bool ProcessRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, UOSInt subReqLen);
		public:
			WebServiceHandler();

			void AddService(const UTF8Char *svcPath, UOSInt svcPathLen, Net::WebUtil::RequestMethod reqMeth, ServiceFunc func);
		};
	}
}
#endif
