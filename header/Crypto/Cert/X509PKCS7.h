#ifndef _SM_CRYPTO_CERT_X509PKCS7
#define _SM_CRYPTO_CERT_X509PKCS7
#include "Crypto/Cert/X509File.h"

namespace Crypto
{
	namespace Cert
	{
		class X509PKCS7 : public Crypto::Cert::X509File
		{
		public:
			X509PKCS7(const UTF8Char *sourceName, const UInt8 *buff, UOSInt buffSize);
			virtual ~X509PKCS7();

			virtual FileType GetFileType();
			virtual void ToShortName(Text::StringBuilderUTF *sb);
			
			virtual ASN1Data *Clone();
			virtual void ToString(Text::StringBuilderUTF *sb);
		};
	}
}
#endif
