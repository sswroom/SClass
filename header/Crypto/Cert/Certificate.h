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

			virtual Crypto::Cert::X509Cert *CreateX509Cert() = 0;
			virtual void ToString(Text::StringBuilderUTF8 *sb) = 0;
		};
	}
}
#endif
