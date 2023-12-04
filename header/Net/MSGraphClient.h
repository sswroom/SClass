#ifndef _SM_NET_MSGRAPHCLIENT
#define _SM_NET_MSGRAPHCLIENT
#include "Data/Timestamp.h"
#include "Net/HTTPClient.h"
#include "Net/SSLEngine.h"
#include "Net/SocketFactory.h"
#include "Text/String.h"

namespace Net
{
	class MSGraphAccessToken
	{
	private:
		NotNullPtr<Text::String> type;
		Data::Timestamp expiresIn;
		Data::Timestamp extExpiresIn;
		NotNullPtr<Text::String> accessToken;

	public:
		MSGraphAccessToken(NotNullPtr<Text::String> type, Int32 expiresIn, Int32 extExpiresIn, NotNullPtr<Text::String> accessToken);
		virtual ~MSGraphAccessToken();

		virtual void InitClient(NotNullPtr<Net::HTTPClient> cli);
	};

	class MSGraphClient
	{
	private:
		NotNullPtr<Net::SocketFactory> sockf;
		Optional<Net::SSLEngine> ssl;

		Optional<MSGraphAccessToken> AccessTokenParse(Net::WebStatus::StatusCode status, Text::CStringNN content);
	public:
		MSGraphClient(NotNullPtr<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl);
		~MSGraphClient();

		Optional<MSGraphAccessToken> AccessTokenGet(Text::CStringNN tenantId, Text::CStringNN clientId, Text::CStringNN clientSecret, Text::CString scope);
		Bool EntityGetMe(NotNullPtr<MSGraphAccessToken> token);
		Bool MailMessagesGet(NotNullPtr<MSGraphAccessToken> token, Text::CStringNN userName, UOSInt top, UOSInt skip);
	};
}
#endif
