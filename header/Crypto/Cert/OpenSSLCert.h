#ifndef _SM_CRYPTO_CERT_OPENSSLCERT
#define _SM_CRYPTO_CERT_OPENSSLCERT
#include "AnyType.h"
#include "Crypto/Cert/Certificate.h"
#include "Crypto/Cert/X509Cert.h"

namespace Crypto
{
	namespace Cert
	{
		class OpenSSLCert : public Certificate
		{
		private:
			struct ClassData;

			NN<ClassData> clsData;
			static Bool FromASN1_TIME(AnyType t, NN<Data::DateTime> dt);
		public:
			OpenSSLCert();
			OpenSSLCert(AnyType x509);
			virtual ~OpenSSLCert();

			Bool GetNotBefore(NN<Data::DateTime> dt) const;
			Bool GetNotAfter(NN<Data::DateTime> dt) const;
			Bool IsSelfSigned() const;

			virtual Optional<Crypto::Cert::X509Cert> CreateX509Cert() const;

			virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
		};
	}
}
#endif
