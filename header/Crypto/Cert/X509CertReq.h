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
			X509CertReq(Text::CStringNN sourceName, Data::ByteArrayR buff);
			virtual ~X509CertReq();

			virtual FileType GetFileType() const;
			virtual void ToShortName(NotNullPtr<Text::StringBuilderUTF8> sb) const;
			virtual ValidStatus IsValid(NotNullPtr<Net::SSLEngine> ssl, Crypto::Cert::CertStore *trustStore) const;
			
			virtual NotNullPtr<ASN1Data> Clone() const;
			virtual void ToString(NotNullPtr<Text::StringBuilderUTF8> sb) const;
			virtual Net::ASN1Names *CreateNames() const;

			Bool GetNames(NotNullPtr<CertNames> names) const;
			Bool GetExtensions(NotNullPtr<CertExtensions> ext) const;
			Crypto::Cert::X509Key *GetNewPublicKey() const;
			Bool GetKeyId(const Data::ByteArray &keyId) const; //20 bytes
		};
	}
}
#endif
