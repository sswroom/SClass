#ifndef _SM_CRYPTO_CERT_X509PRIVKEY
#define _SM_CRYPTO_CERT_X509PRIVKEY
#include "Crypto/Cert/X509File.h"

namespace Crypto
{
	namespace Cert
	{
		class X509PrivKey : public Crypto::Cert::X509File
		{
		public:
			X509PrivKey(const UTF8Char *sourceName, const UInt8 *buff, UOSInt buffSize);
			virtual ~X509PrivKey();

			virtual FileType GetFileType();
			virtual void ToShortName(Text::StringBuilderUTF *sb);

			virtual ASN1Data *Clone();
			virtual void ToString(Text::StringBuilderUTF *sb);

			static X509PrivKey *CreateFromRSAKey(const UInt8 *buff, UOSInt buffSize);
		};
	}
}
#endif
