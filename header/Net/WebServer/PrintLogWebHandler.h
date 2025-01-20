#ifndef _SM_NET_WEBSERVER_PRINTLOGWEBHANDLER
#define _SM_NET_WEBSERVER_PRINTLOGWEBHANDLER
#include "IO/Writer.h"
#include "Net/WebServer/WebHandler.h"

namespace Net
{
	namespace WebServer
	{
		class PrintLogWebHandler : public Net::WebServer::WebHandler
		{
		private:
			NN<Net::WebServer::WebHandler> hdlr;
			NN<IO::Writer> writer;

		public:
			PrintLogWebHandler(NN<Net::WebServer::WebHandler> hdlr, NN<IO::Writer> writer);
			virtual ~PrintLogWebHandler();

			virtual void DoWebRequest(NN<WebRequest> req, NN<WebResponse> resp);
		};
	}
}
#endif
