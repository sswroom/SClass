#ifndef _SM_CRYPTO_CERT_CERTIFICATE
#define _SM_CRYPTO_CERT_CERTIFICATE
#include "Text/StringBuilderUTF8.h"

namespace Crypto
{
	namespace Cert
	{
		class Certificate
		{
		public:
			virtual ~Certificate() {};

			virtual void ToString(Text::StringBuilderUTF8 *sb) = 0;
		};
	}
}
#endif
