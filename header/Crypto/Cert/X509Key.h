#ifndef _SM_CRYPTO_CERT_X509RSAKEY
#define _SM_CRYPTO_CERT_X509RSAKEY
#include "Crypto/Cert/X509File.h"

namespace Crypto
{
	namespace Cert
	{
		class X509Key : public Crypto::Cert::X509File
		{
		private:
			KeyType keyType;
		public:
			X509Key(const UTF8Char *sourceName, const UInt8 *buff, UOSInt buffSize, KeyType keyType);
			virtual ~X509Key();

			virtual FileType GetFileType();
			virtual void ToShortName(Text::StringBuilderUTF *sb);
			
			virtual ASN1Data *Clone();
			virtual void ToString(Text::StringBuilderUTF *sb);

			KeyType GetKeyType();
		};
	}
}
#endif