#ifndef _SM_CRYPTO_CERT_X509PKCS12
#define _SM_CRYPTO_CERT_X509PKCS12
#include "Crypto/Cert/X509File.h"

namespace Crypto
{
	namespace Cert
	{
		class X509PKCS12 : public Crypto::Cert::X509File
		{
		public:
			X509PKCS12(NN<Text::String> sourceName, Data::ByteArrayR buff);
			X509PKCS12(Text::CStringNN sourceName, Data::ByteArrayR buff);
			virtual ~X509PKCS12();

			virtual FileType GetFileType() const;
			virtual void ToShortName(NN<Text::StringBuilderUTF8> sb) const;
			
			virtual UOSInt GetCertCount();
			virtual Bool GetCertName(UOSInt index, NN<Text::StringBuilderUTF8> sb);
			virtual Optional<X509Cert> GetNewCert(UOSInt index);
			virtual ValidStatus IsValid(NN<Net::SSLEngine> ssl, Optional<Crypto::Cert::CertStore> trustStore) const;

			virtual NN<ASN1Data> Clone() const;
			virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
			virtual NN<Net::ASN1Names> CreateNames() const;
		};
	}
}
#endif
