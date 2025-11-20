#ifndef _SM_NET_AZUREMANAGER
#define _SM_NET_AZUREMANAGER
#include "Crypto/Cert/X509Key.h"
#include "Data/FastStringMapNN.hpp"
#include "Net/SSLEngine.h"

namespace Net
{
	class AzureManager
	{
	private:
		NN<Net::TCPClientFactory> clif;
		Optional<Net::SSLEngine> ssl;
		Data::FastStringMapNN<Text::String> *keyMap;

	public:
		AzureManager(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl);
		~AzureManager();

		Optional<Crypto::Cert::X509Key> CreateKey(Text::CStringNN kid);
	};
}
#endif
