#ifndef _SM_NET_WEBSERVER_OAUTH2HANDLER
#define _SM_NET_WEBSERVER_OAUTH2HANDLER
#include "Net/WebServer/WebServiceHandler.h"

namespace Net
{
	namespace WebServer
	{
		class OAuth2Handler : public Net::WebServer::WebServiceHandler
		{
		private:
			static Bool __stdcall AuthHandler(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> me);
			static Bool __stdcall TokenHandler(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> me);
			static Bool __stdcall UserInfoHandler(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> me);
		public:
			OAuth2Handler(Text::CStringNN authPath, Text::CStringNN tokenPath, Text::CStringNN userinfoPath);
			virtual ~OAuth2Handler();
		};
	}
}
#endif
