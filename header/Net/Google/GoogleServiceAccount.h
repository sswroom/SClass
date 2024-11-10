#ifndef _SM_NET_GOOGLE_GOOGLESERVICEACCOUNT
#define _SM_NET_GOOGLE_GOOGLESERVICEACCOUNT
#include "Crypto/Cert/X509PrivKey.h"
#include "Crypto/Token/JWToken.h"
#include "Text/String.h"

namespace Net
{
	namespace Google
	{
		class GoogleServiceAccount
		{
		private:
			NN<Text::String> projectId;
			NN<Text::String> privateKeyId;
			NN<Crypto::Cert::X509PrivKey> privateKey;
			NN<Text::String> clientEmail;
			NN<Text::String> clientId;
			NN<Text::String> authUri;
			NN<Text::String> tokenUri;
			NN<Text::String> authProviderX509CertUrl;
			NN<Text::String> clientX509CertUrl;
			NN<Text::String> universeDomain;

			GoogleServiceAccount(NN<Text::String> projectId, NN<Text::String> privateKeyId, NN<Crypto::Cert::X509PrivKey> privateKey, NN<Text::String> clientEmail, NN<Text::String> clientId, NN<Text::String> authUri, NN<Text::String> tokenUri, NN<Text::String> authProviderX509CertUrl, NN<Text::String> clientX509CertUrl, NN<Text::String> universeDomain);
		public:
			~GoogleServiceAccount();

			NN<Text::String> GetProjectId() const { return this->projectId; }
			NN<Text::String> GetPrivateKeyId() const { return this->privateKeyId; }
			NN<Crypto::Cert::X509PrivKey> GetPrivateKey() const { return this->privateKey; }
			NN<Text::String> GetClientEmail() const { return this->clientEmail; }
			NN<Text::String> GetClientId() const { return this->clientId; }
			NN<Text::String> GetAuthUri() const { return this->authUri; }
			NN<Text::String> GetTokenUri() const { return this->tokenUri; }
			NN<Text::String> GetAuthProviderX509CertUrl() const { return this->authProviderX509CertUrl; }
			NN<Text::String> GetClientX509CertUrl() const { return this->clientX509CertUrl; }
			NN<Text::String> GetUniverseDomain() const { return this->universeDomain; }
			Optional<Crypto::Token::JWToken> ToJWT(Optional<Net::SSLEngine> ssl, Text::CStringNN scope) const;

			static Optional<GoogleServiceAccount> FromFile(Text::CStringNN filePath);
			static Optional<GoogleServiceAccount> FromData(Text::CStringNN fileData);
		};
	}
}
#endif
