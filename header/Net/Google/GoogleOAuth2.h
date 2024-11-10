#ifndef _SM_NET_GOOGLE_GOOGLEOAUTH2
#define _SM_NET_GOOGLE_GOOGLEOAUTH2
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
#include "Net/Google/GoogleServiceAccount.h"
#include "Text/StringBuilderUTF8.h"

namespace Net
{
	namespace Google
	{
		class AccessTokenResponse
		{
		private:
			NN<Text::String> accessToken;
			NN<Text::String> tokenType;
			Int32 expiresIn;
		public:
			AccessTokenResponse(NN<Text::String> accessToken, NN<Text::String> tokenType, Int32 expiresIn) { this->accessToken = accessToken->Clone(); this->tokenType = tokenType->Clone(); this->expiresIn = expiresIn; }
			~AccessTokenResponse() {this->accessToken->Release(); this->tokenType->Release(); }

			NN<Text::String> GetAccessToken() const { return this->accessToken; }
			NN<Text::String> GetTokenType() const { return this->tokenType; }
			Int32 GetExpiresIn() const { return this->expiresIn; }
		};

		class GoogleOAuth2
		{
		private:
			NN<Net::TCPClientFactory> clif;
			Optional<Net::SSLEngine> ssl;

		public:
			GoogleOAuth2(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl) { this->clif = clif; this->ssl = ssl; }
			~GoogleOAuth2() { }

			Optional<AccessTokenResponse> GetServiceToken(NN<GoogleServiceAccount> serviceAccount, Text::CStringNN scope);
		};
	}
}
#endif
