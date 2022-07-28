#ifndef _SM_NET_WEBSERVER_MODBUSDEVSIMHANDLER
#define _SM_NET_WEBSERVER_MODBUSDEVSIMHANDLER
#include "IO/MODBUSDevSim.h"
#include "Net/WebServer/WebStandardHandler.h"

namespace Net
{
	namespace WebServer
	{
		class MODBUSDevSimHandler : public Net::WebServer::WebStandardHandler
		{
		private:
			IO::MODBUSDevSim *dev;
			UOSInt faviconSize;

		protected:
			virtual Bool ProcessRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq);
		public:
			MODBUSDevSimHandler(IO::MODBUSDevSim *dev);
			virtual ~MODBUSDevSimHandler();
		};
	}
}
#endif
