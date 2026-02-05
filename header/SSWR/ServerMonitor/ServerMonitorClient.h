#ifndef _SM_SSWR_SERVERMONITOR_SERVERMONITORCLIENT
#define _SM_SSWR_SERVERMONITOR_SERVERMONITORCLIENT
#include "Net/SSLEngine.h"
#include "SSWR/ServerMonitor/ServerMonitorData.h"
#include "Text/StringBuilderUTF8.h"

namespace SSWR
{
	namespace ServerMonitor
	{
		class ServerMonitorClient
		{
		public:
			virtual ~ServerMonitorClient() {};

			virtual Bool HasError() const = 0;
			virtual Bool ServerValid() = 0;
			virtual Bool BuildTarget(NN<Text::StringBuilderUTF8> target) = 0;
			
			static Optional<ServerMonitorClient> CreateClient(ServerType serverType, NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CStringNN target, Int32 timeoutMS);
		};
	}
}
#endif