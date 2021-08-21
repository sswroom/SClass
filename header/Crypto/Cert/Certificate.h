#ifndef _SM_CRYPTO_CERT_CERTIFICATE
#define _SM_CRYPTO_CERT_CERTIFICATE
#include "Text/StringBuilderUTF.h"

namespace Crypto
{
	namespace Cert
	{
		class Certificate
		{
		public:
			virtual ~Certificate() {};

			virtual void ToString(Text::StringBuilderUTF *sb) = 0;
		};
	}
}
#endif
