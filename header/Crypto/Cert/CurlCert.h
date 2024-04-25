#ifndef _SM_CRYPTO_CERT_CURLCERT
#define _SM_CRYPTO_CERT_CURLCERT
#include "Crypto/Cert/Certificate.h"
#include "Crypto/Cert/X509Cert.h"

namespace Crypto
{
	namespace Cert
	{
		class CurlCert : public Certificate
		{
		private:
			void *certinfo;
		public:
			CurlCert(void *certinfo);
			virtual ~CurlCert();

			Data::Timestamp GetNotBefore() const;
			Data::Timestamp GetNotAfter() const;
			Bool IsSelfSigned() const;

			virtual Optional<Crypto::Cert::X509Cert> CreateX509Cert() const;

			virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
		};
	}
}
#endif
