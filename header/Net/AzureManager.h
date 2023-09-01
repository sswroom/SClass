#ifndef _SM_NET_AZUREMANAGER
#define _SM_NET_AZUREMANAGER
#include "Crypto/Cert/X509Key.h"
#include "Data/FastStringMap.h"
#include "Net/SSLEngine.h"

namespace Net
{
	class AzureManager
	{
	private:
		NotNullPtr<Net::SocketFactory> sockf;
		Net::SSLEngine *ssl;
		Data::FastStringMap<Text::String*> *keyMap;

	public:
		AzureManager(NotNullPtr<Net::SocketFactory> sockf, Net::SSLEngine *ssl);
		~AzureManager();

		Crypto::Cert::X509Key *CreateKey(Text::CStringNN kid);
	};
}
#endif
