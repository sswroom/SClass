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
			NotNullPtr<Net::WebServer::IWebHandler> hdlr;
			NotNullPtr<IO::Writer> writer;

		public:
			PrintLogWebHandler(NotNullPtr<Net::WebServer::IWebHandler> hdlr, NotNullPtr<IO::Writer> writer);
			virtual ~PrintLogWebHandler();

			virtual void WebRequest(NotNullPtr<IWebRequest> req, NotNullPtr<IWebResponse> resp);
		};
	}
}
#endif
