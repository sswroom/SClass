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
		OSInt authBuffSize;
	public:
		HTTPProxyClient(Net::SocketFactory *sockf, Bool noShutdown, UInt32 proxyIP, UInt16 proxyPort);
		virtual ~HTTPProxyClient();

		virtual Bool Connect(const UTF8Char *url, UOSInt urlLen, const Char *method, Double *timeDNS, Double *timeConn, Bool defHeaders);
		Bool SetAuthen(Net::HTTPProxyTCPClient::PasswordType pwdType, const UTF8Char *userName, const UTF8Char *password);
	};
}
#endif
