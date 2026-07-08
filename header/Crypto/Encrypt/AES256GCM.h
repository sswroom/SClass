#ifndef _SM_CRYPTO_ENCRYPT_AES256GCM
#define _SM_CRYPTO_ENCRYPT_AES256GCM
#include "Crypto/Encrypt/BlockCipher.h"

namespace Crypto
{
	namespace Encrypt
	{
		class AES256GCM : public BlockCipher
		{
		private:
			UInt8 key[32];
		public:
			AES256GCM(UnsafeArray<const UInt8> key);
			virtual ~AES256GCM();

			virtual UIntOS Encrypt(UnsafeArray<const UInt8> inBuff, UIntOS inSize, UnsafeArray<UInt8> outBuff);
			virtual UIntOS Decrypt(UnsafeArray<const UInt8> inBuff, UIntOS inSize, UnsafeArray<UInt8> outBuff);

			virtual UIntOS EncryptBlock(UnsafeArray<const UInt8> inBlock, UnsafeArray<UInt8> outBlock) const;
			virtual UIntOS DecryptBlock(UnsafeArray<const UInt8> inBlock, UnsafeArray<UInt8> outBlock) const;
		};
	}
}
#endif
