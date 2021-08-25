#ifndef _SM_NET_SSLCLIENT
#define _SM_NET_SSLCLIENT
#include "Crypto/Cert/Certificate.h"
#include "Net/TCPClient.h"

namespace Net
{
	class SSLClient : public Net::TCPClient
	{
	public:
		SSLClient(Net::SocketFactory *sockf, Socket *s);
		virtual ~SSLClient();

		virtual Bool IsSSL();
		virtual Crypto::Cert::Certificate *GetRemoteCert() = 0;
	};
}
#endif
