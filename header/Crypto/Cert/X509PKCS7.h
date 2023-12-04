#ifndef _SM_CRYPTO_CERT_X509PKCS7
#define _SM_CRYPTO_CERT_X509PKCS7
#include "Crypto/Cert/X509File.h"
#include "Crypto/Hash/IHash.h"

namespace Crypto
{
	namespace Cert
	{
		class X509PKCS7 : public Crypto::Cert::X509File
		{
		public:
			X509PKCS7(NotNullPtr<Text::String> sourceName, Data::ByteArrayR buff);
			X509PKCS7(Text::CStringNN sourceName, Data::ByteArrayR buff);
			virtual ~X509PKCS7();

			virtual FileType GetFileType() const;
			virtual void ToShortName(NotNullPtr<Text::StringBuilderUTF8> sb) const;
			
			virtual UOSInt GetCertCount();
			virtual Bool GetCertName(UOSInt index, NotNullPtr<Text::StringBuilderUTF8> sb);
			virtual X509Cert *GetNewCert(UOSInt index);
			virtual ValidStatus IsValid(NotNullPtr<Net::SSLEngine> ssl, Crypto::Cert::CertStore *trustStore) const;

			virtual NotNullPtr<ASN1Data> Clone() const;
			virtual void ToString(NotNullPtr<Text::StringBuilderUTF8> sb) const;
			virtual Net::ASN1Names *CreateNames() const;

			Bool IsSignData() const;
			Crypto::Hash::HashType GetDigestType() const;
			const UInt8 *GetMessageDigest(UOSInt *digestSize) const;
			const UInt8 *GetEncryptedDigest(UOSInt *encSize) const;
		};
	}
}
#endif
