#ifndef _SM_CRYPTO_CERT_X509PUBKEY
#define _SM_CRYPTO_CERT_X509PUBKEY
#include "Crypto/Cert/X509File.h"
#include "Crypto/Cert/X509Key.h"

namespace Crypto
{
	namespace Cert
	{
		class X509PubKey : public Crypto::Cert::X509File
		{
		public:
			X509PubKey(const UTF8Char *sourceName, const UInt8 *buff, UOSInt buffSize);
			virtual ~X509PubKey();

			virtual FileType GetFileType();
			virtual void ToShortName(Text::StringBuilderUTF *sb);

			virtual ASN1Data *Clone();
			virtual void ToString(Text::StringBuilderUTF *sb);
			
			Crypto::Cert::X509Key *CreateKey();

			static X509PubKey *CreateFromKeyBuff(KeyType keyType, const UInt8 *buff, UOSInt buffSize, const UTF8Char *sourceName);
			static X509PubKey *CreateFromKey(Crypto::Cert::X509Key *key);
		};
	}
}
#endif
