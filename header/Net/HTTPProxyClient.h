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
		Optional<Text::String> proxyUser;
		Optional<Text::String> proxyPwd;

		void AddProxyAuthen();
	public:
		HTTPProxyClient(NN<Net::TCPClientFactory> clif, Text::CString userAgent, UInt32 proxyIP, UInt16 proxyPort);
		virtual ~HTTPProxyClient();

		virtual Bool Connect(Text::CStringNN url, Net::WebUtil::RequestMethod method, OptOut<Double> timeDNS, OptOut<Double> timeConn, Bool defHeaders);
		Bool SetAuthen(Net::HTTPProxyTCPClient::PasswordType pwdType, Text::CStringNN userName, Text::CStringNN password);
	};
}
#endif
