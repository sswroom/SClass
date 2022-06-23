#ifndef _SM_CRYPTO_CERT_X509PRIVKEY
#define _SM_CRYPTO_CERT_X509PRIVKEY
#include "Crypto/Cert/X509File.h"
#include "Crypto/Cert/X509Key.h"

namespace Crypto
{
	namespace Cert
	{
		class X509PrivKey : public Crypto::Cert::X509File
		{
		public:
			X509PrivKey(Text::String *sourceName, const UInt8 *buff, UOSInt buffSize);
			X509PrivKey(Text::CString sourceName, const UInt8 *buff, UOSInt buffSize);
			virtual ~X509PrivKey();

			virtual FileType GetFileType();
			virtual void ToShortName(Text::StringBuilderUTF8 *sb);
			virtual ValidStatus IsValid(Net::SSLEngine *ssl, Crypto::Cert::CertStore *trustStore);

			virtual ASN1Data *Clone();
			virtual void ToString(Text::StringBuilderUTF8 *sb);
			
			Crypto::Cert::X509File::KeyType GetKeyType();
			Crypto::Cert::X509Key *CreateKey();

			static X509PrivKey *CreateFromKeyBuff(KeyType keyType, const UInt8 *buff, UOSInt buffSize, Text::String *sourceName);
			static X509PrivKey *CreateFromKey(Crypto::Cert::X509Key *key);
		};
	}
}
#endif
