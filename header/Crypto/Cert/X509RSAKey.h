#ifndef _SM_CRYPTO_CERT_X509RSAKEY
#define _SM_CRYPTO_CERT_X509RSAKEY
#include "Crypto/Cert/X509File.h"

namespace Crypto
{
	namespace Cert
	{
		class X509RSAKey : public Crypto::Cert::X509File
		{
		public:
			X509RSAKey(const UTF8Char *sourceName, const UInt8 *buff, UOSInt buffSize);
			virtual ~X509RSAKey();

			virtual FileType GetFileType();
			virtual ASN1Data *Clone();
			virtual void ToString(Text::StringBuilderUTF *sb);
		};
	}
}
#endif
