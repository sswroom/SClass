#ifndef _SM_NET_WEBSERVER_MODBUSDEVSIMHANDLER
#define _SM_NET_WEBSERVER_MODBUSDEVSIMHANDLER
#include "IO/MODBUSDevSim.h"
#include "Net/MODBUSTCPListener.h"
#include "Net/WebServer/WebStandardHandler.h"

namespace Net
{
	namespace WebServer
	{
		class MODBUSDevSimHandler : public Net::WebServer::WebStandardHandler
		{
		private:
			IO::MODBUSDevSim *dev;
			Net::MODBUSTCPListener *listener;

		protected:
			virtual Bool ProcessRequest(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CString subReq);
		public:
			MODBUSDevSimHandler(Net::MODBUSTCPListener *listener, IO::MODBUSDevSim *dev);
			virtual ~MODBUSDevSimHandler();
		};
	}
}
#endif
