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
		HTTPProxyTCPClient(NN<Net::SocketFactory> sockf, Text::CStringNN proxyHost, UInt16 proxyPort, PasswordType pt, UnsafeArrayOpt<const UTF8Char> userName, UnsafeArrayOpt<const UTF8Char> pwd, Text::CStringNN destHost, UInt16 destPort);
	};
}
#endif
