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
			X509PrivKey(NN<Text::String> sourceName, Data::ByteArrayR buff);
			X509PrivKey(Text::CStringNN sourceName, Data::ByteArrayR buff);
			virtual ~X509PrivKey();

			virtual FileType GetFileType() const;
			virtual void ToShortName(NN<Text::StringBuilderUTF8> sb) const;
			virtual ValidStatus IsValid(NN<Net::SSLEngine> ssl, Optional<Crypto::Cert::CertStore> trustStore) const;

			virtual NN<ASN1Data> Clone() const;
			virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
			virtual NN<Net::ASN1Names> CreateNames() const;
			
			Crypto::Cert::X509File::KeyType GetKeyType() const;
			Optional<Crypto::Cert::X509Key> CreateKey() const;
			Bool GetKeyId(const Data::ByteArray &keyId) const;

			static NN<X509PrivKey> CreateFromKeyBuff(KeyType keyType, UnsafeArray<const UInt8> buff, UIntOS buffSize, Optional<Text::String> sourceName);
			static Optional<X509PrivKey> CreateFromKey(NN<Crypto::Cert::X509Key> key);
		};
	}
}
#endif
