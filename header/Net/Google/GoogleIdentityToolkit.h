#ifndef _SM_NET_GOOGLE_GOOGLEIDENTITYTOOLKIT
#define _SM_NET_GOOGLE_GOOGLEIDENTITYTOOLKIT
#include "Net/SSLEngine.h"
#include "Net/TCPClientFactory.h"

namespace Net
{
	namespace Google
	{
		class TokenResponse
		{
		private:
			NN<Text::String> idToken;
			NN<Text::String> refreshToken;
			Int32 expiresIn;
		public:
			TokenResponse(NN<Text::String> idToken, NN<Text::String> refreshToken, Int32 expiresIn) { this->idToken = idToken->Clone(); this->refreshToken = refreshToken->Clone(); this->expiresIn = expiresIn; }
			virtual ~TokenResponse() { this->idToken->Release(); this->refreshToken->Release(); }

			NN<Text::String> GetIdToken() const { return this->idToken; }
			NN<Text::String> GetRefreshToken() const { return this->refreshToken; }
			Int32 GetExpiresIn() const { return this->expiresIn; }
		};


		class RefreshTokenResponse : public TokenResponse
		{
		private:
			NN<Text::String> tokenType;
			NN<Text::String> userId;
			NN<Text::String> projectId;

		public:
			RefreshTokenResponse(NN<Text::String> idToken, NN<Text::String> refreshToken, Int32 expiresIn, NN<Text::String> tokenType, NN<Text::String> userId, NN<Text::String> projectId) : TokenResponse(idToken, refreshToken, expiresIn) { this->tokenType = tokenType->Clone(); this->userId = userId->Clone(); this->projectId = projectId->Clone(); }
			virtual ~RefreshTokenResponse() { this->tokenType->Release(); this->userId->Release(); this->projectId->Release(); }

			NN<Text::String> GetTokenType() const { return this->tokenType; }
			NN<Text::String> GetUserId() const { return this->userId; }
			NN<Text::String> GetProjectId() const { return this->projectId; }
		};

		class SignUpResponse : public TokenResponse
		{
		private:
			NN<Text::String> email;
			NN<Text::String> localId;

		public:
			SignUpResponse(NN<Text::String> idToken, NN<Text::String> refreshToken, Int32 expiresIn, NN<Text::String> localId, NN<Text::String> email) : TokenResponse(idToken, refreshToken, expiresIn) { this->localId = localId->Clone(); this->email = email->Clone(); }
			virtual ~SignUpResponse() { this->localId->Release(); this->email->Release(); }

			NN<Text::String> GetLocalId() const { return this->localId; }
			NN<Text::String> GetEmail() const { return this->email; }
		};

		class VerifyPasswordResponse : public SignUpResponse
		{
		private:
			NN<Text::String> displayName;
			Bool registered;

		public:
			VerifyPasswordResponse(NN<Text::String> idToken, NN<Text::String> refreshToken, Int32 expiresIn, NN<Text::String> localId, NN<Text::String> email, NN<Text::String> displayName, Bool registered) : SignUpResponse(idToken, refreshToken, expiresIn, localId, email) { this->displayName = displayName->Clone(); this->registered = registered; }
			virtual ~VerifyPasswordResponse() { this->displayName->Release(); }

			NN<Text::String> GetDisplayName() const { return this->displayName; }
			Bool IsRegistered() const { return this->registered; }
		};

		class GoogleIdentityToolkit
		{
		private:
			NN<Text::String> apiKey;
			NN<Net::TCPClientFactory> clif;
			Optional<Net::SSLEngine> ssl;

		public:
			GoogleIdentityToolkit(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CStringNN apiKey) { this->clif = clif; this->ssl = ssl; this->apiKey = Text::String::New(apiKey); }
			~GoogleIdentityToolkit() { 	this->apiKey->Release(); }

			Optional<TokenResponse> SignInWithCustomToken(Text::CStringNN token);
			Optional<RefreshTokenResponse> RefreshToken(Text::CStringNN refreshToken);
			Optional<SignUpResponse> SignUp(Text::CStringNN email, Text::CStringNN password);
			Optional<VerifyPasswordResponse> SignInWithPassword(Text::CStringNN email, Text::CStringNN password);
			Optional<SignUpResponse> SignInAnonymous();
		};
	}
}
#endif
