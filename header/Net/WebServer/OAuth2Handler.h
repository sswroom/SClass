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
			static Bool __stdcall AuthHandler(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, WebServiceHandler *me);
			static Bool __stdcall TokenHandler(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, WebServiceHandler *me);
			static Bool __stdcall UserInfoHandler(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, WebServiceHandler *me);
		public:
			OAuth2Handler(const UTF8Char *authPath, const UTF8Char *tokenPath, const UTF8Char *userinfoPath);
			virtual ~OAuth2Handler();
		};
	}
}
#endif