#ifndef _SM_NET_WEBSERVER_WEBSTANDARDHANDLER
#define _SM_NET_WEBSERVER_WEBSTANDARDHANDLER
#include "Data/ArrayListNN.h"
#include "Data/FastStringMapNN.h"
#include "Net/WebServer/WebHandler.h"

namespace Net
{
	namespace WebServer
	{
		class WebStandardHandler : public Net::WebServer::WebHandler
		{
		protected:
			Data::FastStringMapNN<Net::WebServer::WebStandardHandler> hdlrs;
			Data::ArrayListNN<Net::WebServer::WebStandardHandler> relHdlrs;
			Optional<Text::String> allowOrigin;
			Optional<Text::String> contentSecurityPolicy;
			Optional<Text::String> upgradeInsecureURL;

		protected:
			Bool DoRequest(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq);
			void AddResponseHeaders(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp);
			Bool ResponseJSONStr(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, OSInt cacheAge, Text::CStringNN json);
			Bool ResponseAllowOptions(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, UOSInt maxAge, Text::CStringNN options);
		public:
			WebStandardHandler();
			virtual ~WebStandardHandler();

			virtual void DoWebRequest(NN<WebRequest> req, NN<WebResponse> resp);

			virtual Bool ProcessRequest(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq);

			void HandlePath(Text::CStringNN relativePath, NN<Net::WebServer::WebStandardHandler> hdlr, Bool needRelease);
			void SetAllowOrigin(Text::CString origin);
			void SetContentSecurityPolicy(Text::CString csp);
			void SetUpgradeInsecureURL(Text::CString upgradeInsecureURL);
		};
	}
}
#endif
