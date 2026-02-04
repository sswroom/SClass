#ifndef _SM_SSWR_SERVERMONITOR_URLMONITORCLIENT
#define _SM_SSWR_SERVERMONITOR_URLMONITORCLIENT
#include "Net/SSLEngine.h"
#include "SSWR/ServerMonitor/ServerMonitorClient.h"
#include "Text/CString.h"

namespace SSWR
{
	namespace ServerMonitor
	{
		class URLMonitorClient : public ServerMonitorClient
		{
		private:
			NN<Net::TCPClientFactory> clif;
			Optional<Net::SSLEngine> ssl;
			NN<Text::String> url;
			Optional<Text::String> containsText;
			Int32 timeoutMS;
		public:
			URLMonitorClient(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CStringNN target, Int32 timeoutMS);
			URLMonitorClient(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CStringNN url, Text::CString containsText, Int32 timeoutMS);
			virtual ~URLMonitorClient();

			virtual Bool HasError() const;
			virtual Bool ServerValid();
			virtual Bool BuildTarget(NN<Text::StringBuilderUTF8> target);
		};
	}
}
#endif