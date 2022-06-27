#ifndef _SM_CRYPTO_ENCRYPT_RC2CIPHER
#define _SM_CRYPTO_ENCRYPT_RC2CIPHER
#include "Crypto/Encrypt/BlockCipher.h"

namespace Crypto
{
	namespace Encrypt
	{
		class RC2Cipher : public BlockCipher
		{
		private:
			static UInt8 permute[256];

			UInt16 xkeys[64];
		public:
			RC2Cipher(const UInt8 *key, UOSInt keyLen);
			virtual ~RC2Cipher();

			Bool SetKey(const UInt8 *key, UOSInt keyLen);
			Bool SetKey(const UInt8 *key, UOSInt keyLen, UOSInt effectiveBits);

			virtual UOSInt EncryptBlock(const UInt8 *inBlock, UInt8 *outBlock, void *encParam) const; //return outSize
			virtual UOSInt DecryptBlock(const UInt8 *inBlock, UInt8 *outBlock, void *decParam) const; //return outSize
		};
	}
}
#endif
