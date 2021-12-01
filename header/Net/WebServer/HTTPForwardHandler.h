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
			Data::ArrayList<const UTF8Char *> *injHeaders;
			UOSInt nextURL;
			Sync::Mutex *mut;
			Net::SocketFactory *sockf;
			Net::SSLEngine *ssl;
			Bool xForwardHeaders;
			Bool locationRemap;

			virtual const UTF8Char *GetNextURL(Net::WebServer::IWebRequest *req);
		public:
			HTTPForwardHandler(Net::SocketFactory *sockf, Net::SSLEngine *ssl, const UTF8Char *forwardURL);
		protected:
			virtual ~HTTPForwardHandler();
		public:
			virtual Bool ProcessRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq);

			void SetXForwardHeaders(Bool xForwardHeaders);
			void SetLocationRemap(Bool locationRemap);
			void AddForwardURL(const UTF8Char *url);
			void AddInjectHeader(const UTF8Char *header);
		};
	}
}
#endif
