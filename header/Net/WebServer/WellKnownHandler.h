#ifndef _SM_NET_WEBSERVER_WELLKNOWNHANDLER
#define _SM_NET_WEBSERVER_WELLKNOWNHANDLER
#include "Data/FastStringMapNN.hpp"
#include "Net/WebServer/WebServiceHandler.h"

namespace Net
{
	namespace WebServer
	{
		class WellKnownHandler : public Net::WebServer::WebServiceHandler
		{
		private:
			Sync::Mutex acmeMut;
			Data::FastStringMapNN<Text::String> acmeMap;
		protected:
			virtual Bool ProcessRequest(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq);
		private:
			static Bool __stdcall AddFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> me);
		public:
			WellKnownHandler();
			virtual ~WellKnownHandler();
		};
	}
}
#endif
