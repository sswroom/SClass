#ifndef _SM_NET_WEBSERVER_WEBREQUESTLOGGER
#define _SM_NET_WEBSERVER_WEBREQUESTLOGGER
#include "Net/WebServer/WebRequest.h"

namespace Net
{
	namespace WebServer
	{
		class WebRequestLogger
		{
		public:
			virtual ~WebRequestLogger(){};

			virtual void LogRequest(NN<WebRequest> req) = 0;
		};
	}
}
#endif
