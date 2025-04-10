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
			RC2Cipher(UnsafeArray<const UInt8> key, UOSInt keyLen);
			virtual ~RC2Cipher();

			Bool SetKey(UnsafeArray<const UInt8> key, UOSInt keyLen);
			Bool SetKey(UnsafeArray<const UInt8> key, UOSInt keyLen, UOSInt effectiveBits);

			virtual UOSInt EncryptBlock(UnsafeArray<const UInt8> inBlock, UnsafeArray<UInt8> outBlock) const; //return outSize
			virtual UOSInt DecryptBlock(UnsafeArray<const UInt8> inBlock, UnsafeArray<UInt8> outBlock) const; //return outSize
		};
	}
}
#endif
