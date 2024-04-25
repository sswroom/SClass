#ifndef _SM_NET_WEBSERVER_IWEBHANDLER
#define _SM_NET_WEBSERVER_IWEBHANDLER
#include "Net/WebServer/IWebRequest.h"
#include "Net/WebServer/IWebResponse.h"

namespace Net
{
	namespace WebServer
	{
		class IWebHandler
		{
		public:
			virtual ~IWebHandler() {};

			virtual void WebRequest(NN<IWebRequest> req, NN<IWebResponse> resp) = 0;
		};
	}
}
#endif
