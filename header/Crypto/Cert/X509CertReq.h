#ifndef _SM_CRYPTO_CERT_X509CERTREQ
#define _SM_CRYPTO_CERT_X509CERTREQ
#include "Crypto/Cert/X509File.h"

namespace Crypto
{
	namespace Cert
	{
		class X509CertReq : public Crypto::Cert::X509File
		{
		public:
			X509CertReq(const UTF8Char *sourceName, const UInt8 *buff, UOSInt buffSize);
			virtual ~X509CertReq();

			virtual FileType GetFileType();
			virtual ASN1Data *Clone();
			virtual void ToString(Text::StringBuilderUTF *sb);
		};
	}
}
#endif
