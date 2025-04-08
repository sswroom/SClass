#ifndef _SM_NET_WEBPUSHCLIENT
#define _SM_NET_WEBPUSHCLIENT
#include "Crypto/Cert/X509Key.h"
#include "Net/SSLEngine.h"
#include "Net/TCPClientFactory.h"

namespace Net
{
	class WebPushClient
	{

	private:
		NN<Net::TCPClientFactory> clif;
		NN<Net::SSLEngine> ssl;

	public:
		WebPushClient(NN<Net::TCPClientFactory> clif, NN<Net::SSLEngine> ssl);
		~WebPushClient();

		Bool Push(Text::CStringNN endPoint, NN<Crypto::Cert::X509Key> key, Text::CStringNN email, Text::CStringNN data, UOSInt ttl) const;
	};
}
#endif
