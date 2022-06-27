#ifndef _SM_CRYPTO_ENCRYPT_AES256
#define _SM_CRYPTO_ENCRYPT_AES256
#include "Crypto/Encrypt/BlockCipher.h"

namespace Crypto
{
	namespace Encrypt
	{
		class AES256 : public Crypto::Encrypt::BlockCipher
		{
		private:
			UInt32 encRK[60];
			UInt32 decRK[60];
		public:
			AES256(const UInt8 *key);
			virtual ~AES256();

			virtual UOSInt EncryptBlock(const UInt8 *inBlock, UInt8 *outBlock, void *encParam) const;
			virtual UOSInt DecryptBlock(const UInt8 *inBlock, UInt8 *outBlock, void *decParam) const;

			void SetKey(const UInt8 *key);
		};
	}
}
#endif
