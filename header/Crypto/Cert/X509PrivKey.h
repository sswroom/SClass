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
			X509PrivKey(NotNullPtr<Text::String> sourceName, Data::ByteArrayR buff);
			X509PrivKey(Text::CStringNN sourceName, Data::ByteArrayR buff);
			virtual ~X509PrivKey();

			virtual FileType GetFileType() const;
			virtual void ToShortName(NotNullPtr<Text::StringBuilderUTF8> sb) const;
			virtual ValidStatus IsValid(NotNullPtr<Net::SSLEngine> ssl, Optional<Crypto::Cert::CertStore> trustStore) const;

			virtual NotNullPtr<ASN1Data> Clone() const;
			virtual void ToString(NotNullPtr<Text::StringBuilderUTF8> sb) const;
			virtual NotNullPtr<Net::ASN1Names> CreateNames() const;
			
			Crypto::Cert::X509File::KeyType GetKeyType() const;
			Optional<Crypto::Cert::X509Key> CreateKey() const;

			static NotNullPtr<X509PrivKey> CreateFromKeyBuff(KeyType keyType, const UInt8 *buff, UOSInt buffSize, Text::String *sourceName);
			static Optional<X509PrivKey> CreateFromKey(NotNullPtr<Crypto::Cert::X509Key> key);
		};
	}
}
#endif
