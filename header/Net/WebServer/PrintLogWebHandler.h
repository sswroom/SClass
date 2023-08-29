#ifndef _SM_NET_WEBSERVER_PRINTLOGWEBHANDLER
#define _SM_NET_WEBSERVER_PRINTLOGWEBHANDLER
#include "IO/Writer.h"
#include "Net/WebServer/IWebHandler.h"

namespace Net
{
	namespace WebServer
	{
		class PrintLogWebHandler : public Net::WebServer::IWebHandler
		{
		private:
			Net::WebServer::IWebHandler *hdlr;
			NotNullPtr<IO::Writer> writer;

		private:
			virtual ~PrintLogWebHandler();
		public:
			PrintLogWebHandler(Net::WebServer::IWebHandler *hdlr, NotNullPtr<IO::Writer> writer);

			virtual void WebRequest(NotNullPtr<IWebRequest> req, NotNullPtr<IWebResponse> resp);
			virtual void Release();
		};
	}
}
#endif
