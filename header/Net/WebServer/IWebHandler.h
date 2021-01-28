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
			virtual void WebRequest(IWebRequest *req, IWebResponse *resp) = 0;
			virtual void Release() = 0;
		};
	};
};
#endif
