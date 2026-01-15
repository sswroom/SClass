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
			X509PubKey(NN<Text::String> sourceName, Data::ByteArrayR buff);
			X509PubKey(Text::CStringNN sourceName, Data::ByteArrayR buff);
			virtual ~X509PubKey();

			virtual FileType GetFileType() const;
			virtual void ToShortName(NN<Text::StringBuilderUTF8> sb) const;
			virtual ValidStatus IsValid(NN<Net::SSLEngine> ssl, Optional<Crypto::Cert::CertStore> trustStore) const;

			virtual NN<ASN1Data> Clone() const;
			virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
			virtual NN<Net::ASN1Names> CreateNames() const;
			
			Optional<Crypto::Cert::X509Key> CreateKey() const;
			Bool GetKeyId(const Data::ByteArray &keyId) const;
			KeyType GetKeyType() const;

			static NN<X509PubKey> CreateFromKeyBuff(KeyType keyType, UnsafeArray<const UInt8> buff, UIntOS buffSize, NN<Text::String> sourceName);
			static NN<X509PubKey> CreateFromKey(NN<Crypto::Cert::X509Key> key);
		};
	}
}
#endif
