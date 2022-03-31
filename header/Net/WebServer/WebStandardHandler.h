#ifndef _SM_NET_WEBSERVER_WEBSTANDARDHANDLER
#define _SM_NET_WEBSERVER_WEBSTANDARDHANDLER
#include "Data/FastStringMap.h"
#include "Net/WebServer/IWebHandler.h"

namespace Net
{
	namespace WebServer
	{
		class WebStandardHandler : public Net::WebServer::IWebHandler
		{
		protected:
			Data::FastStringMap<Net::WebServer::WebStandardHandler *> *hdlrs;
			Data::ArrayList<Net::WebServer::WebStandardHandler*> *relHdlrs;

		protected:
			virtual ~WebStandardHandler();
			Bool DoRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq);
		public:
			WebStandardHandler();

			virtual void WebRequest(IWebRequest *req, IWebResponse *resp);
			virtual void Release();

			virtual Bool ProcessRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq);

			void HandlePath(const UTF8Char *absolutePath, UOSInt pathLen, Net::WebServer::WebStandardHandler *hdlr, Bool needRelease);
		};
	}
}
#endif
