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
			X509CertReq(NN<Text::String> sourceName, Data::ByteArrayR buff);
			X509CertReq(Text::CStringNN sourceName, Data::ByteArrayR buff);
			virtual ~X509CertReq();

			virtual FileType GetFileType() const;
			virtual void ToShortName(NN<Text::StringBuilderUTF8> sb) const;
			virtual ValidStatus IsValid(NN<Net::SSLEngine> ssl, Optional<Crypto::Cert::CertStore> trustStore) const;
			
			virtual NN<ASN1Data> Clone() const;
			virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
			virtual NN<Net::ASN1Names> CreateNames() const;

			Bool GetNames(NN<CertNames> names) const;
			Bool GetExtensions(NN<CertExtensions> ext) const;
			Optional<Crypto::Cert::X509Key> GetNewPublicKey() const;
			Bool GetKeyId(const Data::ByteArray &keyId) const; //20 bytes
		};
	}
}
#endif
