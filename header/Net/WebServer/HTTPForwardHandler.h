#ifndef _SM_NET_WEBSERVER_HTTPFORWARDHANDLER
#define _SM_NET_WEBSERVER_HTTPFORWARDHANDLER
#include "Net/SSLEngine.h"
#include "Net/WebServer/WebStandardHandler.h"
#include "Text/String.h"

namespace Net
{
	namespace WebServer
	{
		class HTTPForwardHandler : public Net::WebServer::WebStandardHandler
		{
		public:
			enum class ForwardType
			{
				Normal,
				Transparent
			};

			typedef void (__stdcall *ReqHandler)(void *userObj, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp);
		private:
			Data::ArrayList<Text::String *> *forwardAddrs;
			Data::ArrayList<Text::String *> *injHeaders;
			UOSInt nextURL;
			Sync::Mutex *mut;
			Net::SocketFactory *sockf;
			Net::SSLEngine *ssl;
			ForwardType fwdType;
			ReqHandler reqHdlr;
			void *reqHdlrObj;

			virtual Text::String *GetNextURL(Net::WebServer::IWebRequest *req);
		public:
			HTTPForwardHandler(Net::SocketFactory *sockf, Net::SSLEngine *ssl, const UTF8Char *forwardURL, ForwardType fwdType);
		protected:
			virtual ~HTTPForwardHandler();
		public:
			virtual Bool ProcessRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, UOSInt subReqLen);

			void AddForwardURL(const UTF8Char *url);
			void AddInjectHeader(Text::String *header);
			void AddInjectHeader(const UTF8Char *header);

			void HandleForwardRequest(ReqHandler reqHdlr, void *userObj);
		};
	}
}
#endif
