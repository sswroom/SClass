#ifndef _SM_CRYPTO_CERT_X509CRL
#define _SM_CRYPTO_CERT_X509CRL
#include "Crypto/Cert/X509File.h"

namespace Crypto
{
	namespace Cert
	{
		class X509CRL : public Crypto::Cert::X509File
		{
		public:
			X509CRL(NN<Text::String> sourceName, Data::ByteArrayR buff);
			X509CRL(Text::CStringNN sourceName, Data::ByteArrayR buff);
			virtual ~X509CRL();

			virtual FileType GetFileType() const;
			virtual void ToShortName(NN<Text::StringBuilderUTF8> sb) const;
			virtual ValidStatus IsValid(NN<Net::SSLEngine> ssl, Optional<Crypto::Cert::CertStore> trustStore) const;
			
			virtual NN<ASN1Data> Clone() const;
			virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
			virtual NN<Net::ASN1Names> CreateNames() const;

			Bool HasVersion() const;
			Bool GetIssuerCN(NN<Text::StringBuilderUTF8> sb) const;
			Bool GetThisUpdate(NN<Data::DateTime> dt) const;
			Bool GetNextUpdate(NN<Data::DateTime> dt) const;
			Bool IsRevoked(NN<Crypto::Cert::X509Cert> cert) const;
		};
	}
}
#endif
