#ifndef _SM_NET_SSLCLIENT
#define _SM_NET_SSLCLIENT
#include "Crypto/X509File.h"
#include "Net/TCPClient.h"

namespace Net
{
	class SSLClient : public Net::TCPClient
	{
	public:
		SSLClient(Net::SocketFactory *sockf, UInt32 *s);
		virtual ~SSLClient();

		virtual Bool IsSSL();
		virtual Crypto::X509File *GetRemoteCert() = 0;
	};
}
#endif
