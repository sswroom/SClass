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
			X509CRL(Text::CString sourceName, Data::ByteArrayR buff);
			virtual ~X509CRL();

			virtual FileType GetFileType() const;
			virtual void ToShortName(Text::StringBuilderUTF8 *sb) const;
			virtual ValidStatus IsValid(Net::SSLEngine *ssl, Crypto::Cert::CertStore *trustStore) const;
			
			virtual ASN1Data *Clone() const;
			virtual void ToString(Text::StringBuilderUTF8 *sb) const;

			Bool HasVersion() const;
			Bool GetIssuerCN(Text::StringBuilderUTF8 *sb) const;
			Bool GetThisUpdate(Data::DateTime *dt) const;
			Bool GetNextUpdate(Data::DateTime *dt) const;
			Bool IsRevoked(Crypto::Cert::X509Cert *cert) const;
		};
	}
}
#endif
