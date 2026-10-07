#ifndef _SM_NET_SSLCLIENT
#define _SM_NET_SSLCLIENT
#include "Crypto/Cert/Certificate.h"
#include "Net/TCPClient.h"

namespace Net
{
	class SSLClient : public Net::TCPClient
	{
	public:
		SSLClient(NN<Net::SocketFactory> sockf, NN<Socket> s);
		virtual ~SSLClient();

		virtual Bool IsSSL() const;
		virtual IO::StreamType GetStreamType() const;
		virtual Optional<Crypto::Cert::Certificate> GetRemoteCert() = 0;
		virtual Optional<const Data::ReadingListNN<Crypto::Cert::Certificate>> GetRemoteCerts() = 0;
	};
}
#endif
