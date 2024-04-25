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
			NN<Net::WebServer::IWebHandler> hdlr;
			NN<IO::Writer> writer;

		public:
			PrintLogWebHandler(NN<Net::WebServer::IWebHandler> hdlr, NN<IO::Writer> writer);
			virtual ~PrintLogWebHandler();

			virtual void WebRequest(NN<IWebRequest> req, NN<IWebResponse> resp);
		};
	}
}
#endif
