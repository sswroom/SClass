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
		UInt8 *authBuff;
		UOSInt authBuffSize;
	public:
		HTTPProxyClient(NN<Net::SocketFactory> sockf, Bool noShutdown, UInt32 proxyIP, UInt16 proxyPort);
		virtual ~HTTPProxyClient();

		virtual Bool Connect(Text::CStringNN url, Net::WebUtil::RequestMethod method, Double *timeDNS, Double *timeConn, Bool defHeaders);
		Bool SetAuthen(Net::HTTPProxyTCPClient::PasswordType pwdType, const UTF8Char *userName, const UTF8Char *password);
	};
}
#endif
