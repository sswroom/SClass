#ifndef _SM_NET_HTTPPROXYCLIENT
#define _SM_NET_HTTPPROXYCLIENT
#include "Net/HTTPMyClient.h"
#include "Net/HTTPProxyTCPClient.h"

namespace Net
{
	class HTTPProxyClient : public HTTPMyClient
	{
	private:
		UInt32 proxyIP;
		UInt16 proxyPort;
	public:
		HTTPProxyClient(NN<Net::TCPClientFactory> clif, Bool noShutdown, UInt32 proxyIP, UInt16 proxyPort);
		virtual ~HTTPProxyClient();

		virtual Bool Connect(Text::CStringNN url, Net::WebUtil::RequestMethod method, OptOut<Double> timeDNS, OptOut<Double> timeConn, Bool defHeaders);
		Bool SetAuthen(Net::HTTPProxyTCPClient::PasswordType pwdType, UnsafeArray<const UTF8Char> userName, UnsafeArray<const UTF8Char> password);
	};
}
#endif
