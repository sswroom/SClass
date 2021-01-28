#ifndef _SM_CRYPTO_ENCRYPT_AES128
#define _SM_CRYPTO_ENCRYPT_AES128
#include "Crypto/Encrypt/BlockCipher.h"

namespace Crypto
{
	namespace Encrypt
	{
		class AES128 : public Crypto::Encrypt::BlockCipher
		{
		private:
			UInt32 encRK[44];
			UInt32 decRK[44];
		public:
			AES128(const UInt8 *key);
			virtual ~AES128();

			virtual OSInt EncryptBlock(const UInt8 *inBlock, UInt8 *outBlock, void *encParam);
			virtual OSInt DecryptBlock(const UInt8 *inBlock, UInt8 *outBlock, void *decParam);

			void SetKey(const UInt8 *key);
		};
	}
}
#endif
