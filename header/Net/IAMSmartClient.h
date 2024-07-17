#ifndef _SM_NET_IAMSMARTCLIENT
#define _SM_NET_IAMSMARTCLIENT
#include "Net/IAMSmartAPI.h"

namespace Net
{
	class IAMSmartClient
	{
	private:
		IAMSmartAPI api;
		IAMSmartAPI::CEKInfo cek;
		Optional<Crypto::Cert::X509PrivKey> key;

		Bool PrepareCEK();
	public:
		IAMSmartClient(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Text::CStringNN domain, Text::CStringNN clientID, Text::CStringNN clientSecret, Text::CStringNN keyFile);
		~IAMSmartClient();

		Bool IsError() const;
		Bool GetToken(Text::CStringNN code, NN<IAMSmartAPI::TokenInfo> token);
		Bool GetProfiles(NN<IAMSmartAPI::TokenInfo> token, Text::CStringNN eMEFields, Text::CStringNN profileFields, NN<IAMSmartAPI::ProfileInfo> profiles);
	};
}
#endif
