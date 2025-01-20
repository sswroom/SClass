#ifndef _SM_NET_WEBSERVER_WEBHANDLER
#define _SM_NET_WEBSERVER_WEBHANDLER
#include "Net/WebServer/WebRequest.h"
#include "Net/WebServer/WebResponse.h"

namespace Net
{
	namespace WebServer
	{
		class WebHandler
		{
		public:
			virtual ~WebHandler() {};

			virtual void DoWebRequest(NN<WebRequest> req, NN<WebResponse> resp) = 0;
		};
	}
}
#endif
