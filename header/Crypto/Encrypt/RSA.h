#ifndef _SM_CRYPTO_ENCRYPT_RSA
#define _SM_CRYPTO_ENCRYPT_RSA
#include "Crypto/Encrypt/BlockCipher.h"

namespace Crypto
{
	namespace Encrypt
	{
		class RSA : public Crypto::Encrypt::BlockCipher
		{
		public:
			RSA();
			virtual ~RSA();
		};
	}
}
#endif
