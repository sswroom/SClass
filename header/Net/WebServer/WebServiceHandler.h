#ifndef _SM_NET_WEBSERVER_WEBSERVICEHANDLER
#define _SM_NET_WEBSERVER_WEBSERVICEHANDLER
#include "Data/Integer32Map.h"
#include "Data/StringUTF8Map.h"
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
				const UTF8Char *svcPath;
				Data::Integer32Map<ServiceFunc> *funcs;
			} ServiceInfo;
			
			Data::StringUTF8Map<ServiceInfo *> *services;

		protected:
			virtual ~WebServiceHandler();
			virtual Bool ProcessRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq);
		public:
			WebServiceHandler();

			void AddService(const UTF8Char *svcPath, Net::WebServer::IWebRequest::RequestMethod reqMeth, ServiceFunc func);
		};
	}
}
#endif
