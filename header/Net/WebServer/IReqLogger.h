#ifndef _SM_NET_WEBSERVER_IREQLOGGER
#define _SM_NET_WEBSERVER_IREQLOGGER
#include "Net/WebServer/IWebRequest.h"

namespace Net
{
	namespace WebServer
	{
		class IReqLogger
		{
		public:
			virtual ~IReqLogger(){};

			virtual void LogRequest(NotNullPtr<IWebRequest> req) = 0;
		};
	}
}
#endif
