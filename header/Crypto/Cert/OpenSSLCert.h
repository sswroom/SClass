#ifndef _SM_CRYPTO_CERT_OPENSSLCERT
#define _SM_CRYPTO_CERT_OPENSSLCERT
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

			ClassData *clsData;
			static Bool FromASN1_TIME(void *t, Data::DateTime *dt);
		public:
			OpenSSLCert();
			OpenSSLCert(void *x509);
			virtual ~OpenSSLCert();

			Bool GetNotBefore(Data::DateTime *dt) const;
			Bool GetNotAfter(Data::DateTime *dt) const;
			Bool IsSelfSigned() const;

			virtual Optional<Crypto::Cert::X509Cert> CreateX509Cert() const;

			virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
		};
	}
}
#endif
