#ifndef _SM_NET_WEBSERVER_WEBSTANDARDHANDLER
#define _SM_NET_WEBSERVER_WEBSTANDARDHANDLER
#include "Data/ArrayListNN.h"
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
			Data::ArrayListNN<Net::WebServer::WebStandardHandler> relHdlrs;
			Optional<Text::String> allowOrigin;

		protected:
			Bool DoRequest(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq);
			void AddResponseHeaders(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp);
			Bool ResponseJSONStr(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, OSInt cacheAge, Text::CStringNN json);
			Bool ResponseAllowOptions(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, UOSInt maxAge, Text::CStringNN options);
		public:
			WebStandardHandler();
			virtual ~WebStandardHandler();

			virtual void WebRequest(NotNullPtr<IWebRequest> req, NotNullPtr<IWebResponse> resp);

			virtual Bool ProcessRequest(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq);

			void HandlePath(Text::CStringNN relativePath, NotNullPtr<Net::WebServer::WebStandardHandler> hdlr, Bool needRelease);
			void SetAllowOrigin(Text::CString origin);
		};
	}
}
#endif
