#ifndef _SM_NET_WEBSERVER_WEBSTANDARDHANDLER
#define _SM_NET_WEBSERVER_WEBSTANDARDHANDLER
#include "Data/StringUTF8Map.h"
#include "Net/WebServer/IWebHandler.h"

namespace Net
{
	namespace WebServer
	{
		class WebStandardHandler : public Net::WebServer::IWebHandler
		{
		protected:
			Data::StringUTF8Map<Net::WebServer::WebStandardHandler *> *hdlrs;
			Data::ArrayList<Net::WebServer::WebStandardHandler*> *relHdlrs;

		protected:
			virtual ~WebStandardHandler();
			virtual Bool ProcessRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq);
			Bool DoRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq);
		public:
			WebStandardHandler();

			virtual void WebRequest(IWebRequest *req, IWebResponse *resp);
			virtual void Release();

			void HandlePath(const UTF8Char *absolutePath, Net::WebServer::WebStandardHandler *hdlr, Bool needRelease);
		};
	}
}
#endif
