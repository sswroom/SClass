#ifndef _SM_NET_WEBSERVER_WEBSTANDARDHANDLER
#define _SM_NET_WEBSERVER_WEBSTANDARDHANDLER
#include "Data/ArrayListNN.h"
#include "Data/FastStringMapNN.h"
#include "Net/WebServer/IWebHandler.h"

namespace Net
{
	namespace WebServer
	{
		class WebStandardHandler : public Net::WebServer::IWebHandler
		{
		protected:
			Data::FastStringMapNN<Net::WebServer::WebStandardHandler> hdlrs;
			Data::ArrayListNN<Net::WebServer::WebStandardHandler> relHdlrs;
			Optional<Text::String> allowOrigin;

		protected:
			Bool DoRequest(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq);
			void AddResponseHeaders(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp);
			Bool ResponseJSONStr(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, OSInt cacheAge, Text::CStringNN json);
			Bool ResponseAllowOptions(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, UOSInt maxAge, Text::CStringNN options);
		public:
			WebStandardHandler();
			virtual ~WebStandardHandler();

			virtual void WebRequest(NN<IWebRequest> req, NN<IWebResponse> resp);

			virtual Bool ProcessRequest(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq);

			void HandlePath(Text::CStringNN relativePath, NN<Net::WebServer::WebStandardHandler> hdlr, Bool needRelease);
			void SetAllowOrigin(Text::CString origin);
		};
	}
}
#endif
