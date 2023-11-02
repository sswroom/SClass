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
			X509CRL(NotNullPtr<Text::String> sourceName, Data::ByteArrayR buff);
			X509CRL(Text::CStringNN sourceName, Data::ByteArrayR buff);
			virtual ~X509CRL();

			virtual FileType GetFileType() const;
			virtual void ToShortName(NotNullPtr<Text::StringBuilderUTF8> sb) const;
			virtual ValidStatus IsValid(Net::SSLEngine *ssl, Crypto::Cert::CertStore *trustStore) const;
			
			virtual NotNullPtr<ASN1Data> Clone() const;
			virtual void ToString(NotNullPtr<Text::StringBuilderUTF8> sb) const;
			virtual Net::ASN1Names *CreateNames() const;

			Bool HasVersion() const;
			Bool GetIssuerCN(NotNullPtr<Text::StringBuilderUTF8> sb) const;
			Bool GetThisUpdate(NotNullPtr<Data::DateTime> dt) const;
			Bool GetNextUpdate(NotNullPtr<Data::DateTime> dt) const;
			Bool IsRevoked(NotNullPtr<Crypto::Cert::X509Cert> cert) const;
		};
	}
}
#endif
