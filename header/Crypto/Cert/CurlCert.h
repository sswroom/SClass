#ifndef _SM_CRYPTO_CERT_CURLCERT
#define _SM_CRYPTO_CERT_CURLCERT
#include "Crypto/Cert/Certificate.h"

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

			virtual void ToString(Text::StringBuilderUTF *sb);
		};
	}
}
#endif
