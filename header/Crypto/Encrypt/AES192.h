#ifndef _SM_CRYPTO_ENCRYPT_AES192
#define _SM_CRYPTO_ENCRYPT_AES192
#include "Crypto/Encrypt/BlockCipher.h"

namespace Crypto
{
	namespace Encrypt
	{
		class AES192 : public Crypto::Encrypt::BlockCipher
		{
		private:
			UInt32 encRK[52];
			UInt32 decRK[52];
		public:
			AES192(const UInt8 *key);
			virtual ~AES192();

			virtual OSInt EncryptBlock(const UInt8 *inBlock, UInt8 *outBlock, void *encParam);
			virtual OSInt DecryptBlock(const UInt8 *inBlock, UInt8 *outBlock, void *decParam);

			void SetKey(const UInt8 *key);
		};
	}
}
#endif
