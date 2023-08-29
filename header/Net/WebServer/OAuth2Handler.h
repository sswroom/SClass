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
			static Bool __stdcall AuthHandler(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CString subReq, WebServiceHandler *me);
			static Bool __stdcall TokenHandler(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CString subReq, WebServiceHandler *me);
			static Bool __stdcall UserInfoHandler(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CString subReq, WebServiceHandler *me);
		public:
			OAuth2Handler(Text::CString authPath, Text::CString tokenPath, Text::CString userinfoPath);
			virtual ~OAuth2Handler();
		};
	}
}
#endif
