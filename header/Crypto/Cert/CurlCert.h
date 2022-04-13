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

			Bool GetNotBefore(Data::DateTime *dt);
			Bool GetNotAfter(Data::DateTime *dt);
			Bool IsSelfSigned();

			virtual Crypto::Cert::X509Cert *CreateX509Cert();

			virtual void ToString(Text::StringBuilderUTF8 *sb);
		};
	}
}
#endif
