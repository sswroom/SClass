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
			X509PubKey(NotNullPtr<Text::String> sourceName, Data::ByteArrayR buff);
			X509PubKey(Text::CStringNN sourceName, Data::ByteArrayR buff);
			virtual ~X509PubKey();

			virtual FileType GetFileType() const;
			virtual void ToShortName(NotNullPtr<Text::StringBuilderUTF8> sb) const;
			virtual ValidStatus IsValid(NotNullPtr<Net::SSLEngine> ssl, Crypto::Cert::CertStore *trustStore) const;

			virtual NotNullPtr<ASN1Data> Clone() const;
			virtual void ToString(NotNullPtr<Text::StringBuilderUTF8> sb) const;
			virtual Net::ASN1Names *CreateNames() const;
			
			Crypto::Cert::X509Key *CreateKey() const;

			static X509PubKey *CreateFromKeyBuff(KeyType keyType, const UInt8 *buff, UOSInt buffSize, NotNullPtr<Text::String> sourceName);
			static X509PubKey *CreateFromKey(NotNullPtr<Crypto::Cert::X509Key> key);
		};
	}
}
#endif
