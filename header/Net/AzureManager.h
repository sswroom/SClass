#ifndef _SM_NET_AZUREMANAGER
#define _SM_NET_AZUREMANAGER
#include "Crypto/Cert/X509Key.h"
#include "Data/FastStringMapNN.h"
#include "Net/SSLEngine.h"

namespace Net
{
	class AzureManager
	{
	private:
		NN<Net::SocketFactory> sockf;
		Optional<Net::SSLEngine> ssl;
		Data::FastStringMapNN<Text::String> *keyMap;

	public:
		AzureManager(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl);
		~AzureManager();

		Crypto::Cert::X509Key *CreateKey(Text::CStringNN kid);
	};
}
#endif
