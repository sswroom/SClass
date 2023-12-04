#ifndef _SM_NET_MSGRAPHCLIENT
#define _SM_NET_MSGRAPHCLIENT
#include "Data/Timestamp.h"
#include "Net/SocketFactory.h"
#include "Text/String.h"

namespace Net
{
	struct MSGraphAccessToken
	{
		NotNullPtr<Text::String> type;
		Data::Timestamp expiresIn;
		Data::Timestamp extExpiresIn;
		NotNullPtr<Text::String> accessToken;
	};

	class MSGraphClient
	{
	private:
		NotNullPtr<Net::SocketFactory> sockf;
		Optional<Net::SSLEngine> ssl;
	public:
		MSGraphClient(NotNullPtr<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl);
		~MSGraphClient();

		Bool GetAccessToken(Text::CStringNN tenantId, Text::CStringNN clientId, Text::CStringNN clientSecret, NotNullPtr<MSGraphAccessToken> tokenOut);
	};
}
#endif
