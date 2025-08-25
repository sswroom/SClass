#ifndef _SM_CRYPTO_CERT_X509EPRIVKEY
#define _SM_CRYPTO_CERT_X509EPRIVKEY
#include "Crypto/Cert/X509File.h"
#include "Crypto/Cert/X509Key.h"

namespace Crypto
{
	namespace Cert
	{
		class X509EPrivKey : public Crypto::Cert::X509File
		{
		public:
			X509EPrivKey(NN<Text::String> sourceName, Data::ByteArrayR buff);
			X509EPrivKey(Text::CStringNN sourceName, Data::ByteArrayR buff);
			virtual ~X509EPrivKey();

			virtual FileType GetFileType() const;
			virtual void ToShortName(NN<Text::StringBuilderUTF8> sb) const;
			virtual ValidStatus IsValid(NN<Net::SSLEngine> ssl, Optional<Crypto::Cert::CertStore> trustStore) const;

			virtual NN<ASN1Data> Clone() const;
			virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
			virtual NN<Net::ASN1Names> CreateNames() const;
		};
	}
}
#endif
