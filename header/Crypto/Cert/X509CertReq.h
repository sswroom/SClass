#ifndef _SM_CRYPTO_CERT_X509CERTREQ
#define _SM_CRYPTO_CERT_X509CERTREQ
#include "Crypto/Cert/X509File.h"
#include "Data/ByteArray.h"

namespace Crypto
{
	namespace Cert
	{
		//PKCS-10 CertificateRequest
		class X509CertReq : public Crypto::Cert::X509File
		{
		public:
			X509CertReq(NotNullPtr<Text::String> sourceName, Data::ByteArrayR buff);
			X509CertReq(Text::CString sourceName, Data::ByteArrayR buff);
			virtual ~X509CertReq();

			virtual FileType GetFileType() const;
			virtual void ToShortName(Text::StringBuilderUTF8 *sb) const;
			virtual ValidStatus IsValid(Net::SSLEngine *ssl, Crypto::Cert::CertStore *trustStore) const;
			
			virtual ASN1Data *Clone() const;
			virtual void ToString(Text::StringBuilderUTF8 *sb) const;

			Bool GetNames(CertNames *names) const;
			Bool GetExtensions(CertExtensions *ext) const;
			Crypto::Cert::X509Key *GetNewPublicKey() const;
			Bool GetKeyId(const Data::ByteArray &keyId) const; //20 bytes
		};
	}
}
#endif
