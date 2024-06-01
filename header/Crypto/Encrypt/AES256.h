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
			AES256(UnsafeArray<const UInt8> key);
			virtual ~AES256();

			virtual UOSInt EncryptBlock(UnsafeArray<const UInt8> inBlock, UnsafeArray<UInt8> outBlock) const;
			virtual UOSInt DecryptBlock(UnsafeArray<const UInt8> inBlock, UnsafeArray<UInt8> outBlock) const;

			void SetKey(UnsafeArray<const UInt8> key);
		};
	}
}
#endif
