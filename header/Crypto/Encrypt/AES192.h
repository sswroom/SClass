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
			AES192(UnsafeArray<const UInt8> key);
			virtual ~AES192();

			virtual UIntOS EncryptBlock(UnsafeArray<const UInt8> inBlock, UnsafeArray<UInt8> outBlock) const;
			virtual UIntOS DecryptBlock(UnsafeArray<const UInt8> inBlock, UnsafeArray<UInt8> outBlock) const;

			void SetKey(UnsafeArray<const UInt8> key);
		};
	}
}
#endif
