#ifndef _SM_CRYPTO_CERT_CERTIFICATE
#define _SM_CRYPTO_CERT_CERTIFICATE
#include "Text/StringBuilderUTF8.h"

namespace Crypto
{
	namespace Cert
	{
		class X509Cert;
		
		class Certificate
		{
		public:
			virtual ~Certificate() {};

			virtual Optional<Crypto::Cert::X509Cert> CreateX509Cert() const = 0;
			virtual void ToString(NN<Text::StringBuilderUTF8> sb) const = 0;
		};
	}
}
#endif
