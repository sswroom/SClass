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
			X509PKCS7(NN<Text::String> sourceName, Data::ByteArrayR buff);
			X509PKCS7(Text::CStringNN sourceName, Data::ByteArrayR buff);
			virtual ~X509PKCS7();

			virtual FileType GetFileType() const;
			virtual void ToShortName(NN<Text::StringBuilderUTF8> sb) const;
			
			virtual UOSInt GetCertCount();
			virtual Bool GetCertName(UOSInt index, NN<Text::StringBuilderUTF8> sb);
			virtual Optional<X509Cert> GetNewCert(UOSInt index);
			virtual ValidStatus IsValid(NN<Net::SSLEngine> ssl, Optional<Crypto::Cert::CertStore> trustStore) const;

			virtual NN<ASN1Data> Clone() const;
			virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
			virtual NN<Net::ASN1Names> CreateNames() const;

			Bool IsSignData() const;
			Crypto::Hash::HashType GetDigestType() const;
			const UInt8 *GetMessageDigest(OutParam<UOSInt> digestSize) const;
			const UInt8 *GetEncryptedDigest(OutParam<UOSInt> encSize) const;
		};
	}
}
#endif
