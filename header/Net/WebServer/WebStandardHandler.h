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
			Data::FastStringMap<Net::WebServer::WebStandardHandler *> hdlrs;
			Data::ArrayList<Net::WebServer::WebStandardHandler*> relHdlrs;

		protected:
			virtual ~WebStandardHandler();
			Bool DoRequest(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CString subReq);
		public:
			WebStandardHandler();

			virtual void WebRequest(NotNullPtr<IWebRequest> req, NotNullPtr<IWebResponse> resp);
			virtual void Release();

			virtual Bool ProcessRequest(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CString subReq);

			void HandlePath(Text::CString relativePath, Net::WebServer::WebStandardHandler *hdlr, Bool needRelease);
		};
	}
}
#endif
