#ifndef _SM_CRYPTO_CERT_X509CERTREQ
#define _SM_CRYPTO_CERT_X509CERTREQ
#include "Crypto/Cert/X509File.h"

namespace Crypto
{
	namespace Cert
	{
		//PKCS-10 CertificateRequest
		class X509CertReq : public Crypto::Cert::X509File
		{
		public:
			X509CertReq(const UTF8Char *sourceName, const UInt8 *buff, UOSInt buffSize);
			virtual ~X509CertReq();

			virtual FileType GetFileType();
			virtual void ToShortName(Text::StringBuilderUTF *sb);
			
			virtual ASN1Data *Clone();
			virtual void ToString(Text::StringBuilderUTF *sb);

			Bool GetNames(CertNames *names);
			Bool GetExtensions(CertExtensions *ext);
			Crypto::Cert::X509Key *GetPublicKey();
			Bool GetKeyId(UInt8 *keyId);
		};
	}
}
#endif
