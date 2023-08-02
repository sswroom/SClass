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
			X509PKCS12(NotNullPtr<Text::String> sourceName, Data::ByteArrayR buff);
			X509PKCS12(Text::CString sourceName, Data::ByteArrayR buff);
			virtual ~X509PKCS12();

			virtual FileType GetFileType() const;
			virtual void ToShortName(NotNullPtr<Text::StringBuilderUTF8> sb) const;
			
			virtual UOSInt GetCertCount();
			virtual Bool GetCertName(UOSInt index, NotNullPtr<Text::StringBuilderUTF8> sb);
			virtual X509Cert *GetNewCert(UOSInt index);
			virtual ValidStatus IsValid(Net::SSLEngine *ssl, Crypto::Cert::CertStore *trustStore) const;

			virtual NotNullPtr<ASN1Data> Clone() const;
			virtual void ToString(NotNullPtr<Text::StringBuilderUTF8> sb) const;
		};
	}
}
#endif
