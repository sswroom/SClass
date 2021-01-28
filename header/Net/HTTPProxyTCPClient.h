#ifndef _SM_NET_HTTPPROXYTCPCLIENT
#define _SM_NET_HTTPPROXYTCPCLIENT
#include "Net/TCPClient.h"

namespace Net
{
	class HTTPProxyTCPClient : public TCPClient
	{
	public:
		typedef enum
		{
			PWDT_NONE,
			PWDT_BASIC
		} PasswordType;

	public:
		HTTPProxyTCPClient(Net::SocketFactory *sockf, const UTF8Char *proxyHost, UInt16 proxyPort, PasswordType pt, const UTF8Char *userName, const UTF8Char *pwd, const UTF8Char *destHost, UInt16 destPort);
	};
}
#endif
