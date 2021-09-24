#ifndef _SM_NET_WEBSERVER_HTTPFORWARDHANDLER
#define _SM_NET_WEBSERVER_HTTPFORWARDHANDLER
#include "Net/SSLEngine.h"
#include "Net/WebServer/WebStandardHandler.h"

namespace Net
{
	namespace WebServer
	{
		class HTTPForwardHandler : public Net::WebServer::WebStandardHandler
		{
		private:
			Data::ArrayList<const UTF8Char *> *forwardAddrs;
			UOSInt nextURL;
			Sync::Mutex *mut;
			Net::SocketFactory *sockf;
			Net::SSLEngine *ssl;

			virtual const UTF8Char *GetNextURL(Net::WebServer::IWebRequest *req);
		public:
			HTTPForwardHandler(Net::SocketFactory *sockf, Net::SSLEngine *ssl, const UTF8Char *forwardURL);
		protected:
			virtual ~HTTPForwardHandler();
		public:
			virtual Bool ProcessRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq);

			void AddForwardURL(const UTF8Char *url);
		};
	}
}
#endif
