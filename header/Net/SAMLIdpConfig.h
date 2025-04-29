#ifndef _SM_NET_SAMLIDPCONFIG
#define _SM_NET_SAMLIDPCONFIG
#include "Crypto/Cert/X509Cert.h"
#include "Net/TCPClientFactory.h"
#include "Text/EncodingFactory.h"

namespace Net
{
	class SAMLIdpConfig
	{
	private:
		NN<Text::String> serviceDispName;
		NN<Text::String> signOnLocation;
		NN<Text::String> logoutLocation;
		Optional<Crypto::Cert::X509Cert> encryptionCert;
		Optional<Crypto::Cert::X509Cert> signingCert;
	public:
		SAMLIdpConfig(Text::CStringNN serviceDispName, Text::CStringNN signOnLocation, Text::CStringNN logoutLocation, Optional<Crypto::Cert::X509Cert> encryptionCert, Optional<Crypto::Cert::X509Cert> signingCert);
		~SAMLIdpConfig();

		NN<Text::String> GetServiceDispName() const { return this->serviceDispName; }
		NN<Text::String> GetSignOnLocation() const { return this->signOnLocation; }
		NN<Text::String> GetLogoutLocation() const { return this->logoutLocation; }
		Optional<Crypto::Cert::X509Cert> GetEncryptionCert() const { return this->encryptionCert; }
		Optional<Crypto::Cert::X509Cert> GetSigningCert() const { return this->signingCert; }

		static Optional<SAMLIdpConfig> ParseMetadata(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, NN<Text::EncodingFactory> encFact, Text::CStringNN path);
	};
}
#endif
