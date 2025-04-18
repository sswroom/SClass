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
			Optional<IO::MODBUSDevSim> dev;
			NN<Net::MODBUSTCPListener> listener;

		protected:
			virtual Bool ProcessRequest(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq);
		public:
			MODBUSDevSimHandler(NN<Net::MODBUSTCPListener> listener, Optional<IO::MODBUSDevSim> dev);
			virtual ~MODBUSDevSimHandler();
		};
	}
}
#endif
