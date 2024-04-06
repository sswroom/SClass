#ifndef _SM_NET_WEBSERVER_WELLKNOWNHANDLER
#define _SM_NET_WEBSERVER_WELLKNOWNHANDLER
#include "Net/WebServer/WebServiceHandler.h"

namespace Net
{
	namespace WebServer
	{
		class WellKnownHandler : public Net::WebServer::WebServiceHandler
		{
		private:
			Sync::Mutex acmeMut;
			Data::FastStringMap<Text::String*> acmeMap;
		protected:
			virtual Bool ProcessRequest(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq);
		private:
			static Bool __stdcall AddFunc(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NotNullPtr<WebServiceHandler> me);
		public:
			WellKnownHandler();
			virtual ~WellKnownHandler();
		};
	}
}
#endif
