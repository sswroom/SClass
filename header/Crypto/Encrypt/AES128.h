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
			AES128(UnsafeArray<const UInt8> key);
			virtual ~AES128();

			virtual UIntOS EncryptBlock(UnsafeArray<const UInt8> inBlock, UnsafeArray<UInt8> outBlock) const;
			virtual UIntOS DecryptBlock(UnsafeArray<const UInt8> inBlock, UnsafeArray<UInt8> outBlock) const;

			void SetKey(UnsafeArray<const UInt8> key);
		};
	}
}
#endif
