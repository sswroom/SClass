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

			Bool GetNotBefore(Data::DateTime *dt) const;
			Bool GetNotAfter(Data::DateTime *dt) const;
			Bool IsSelfSigned() const;

			virtual NotNullPtr<Crypto::Cert::X509Cert> CreateX509Cert() const;

			virtual void ToString(NotNullPtr<Text::StringBuilderUTF8> sb) const;
		};
	}
}
#endif
