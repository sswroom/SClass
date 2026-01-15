#ifndef _SM_CRYPTO_ENCRYPT_AES256GCM
#define _SM_CRYPTO_ENCRYPT_AES256GCM
#include "Crypto/Encrypt/Encryption.h"

namespace Crypto
{
	namespace Encrypt
	{
		class AES256GCM : public Encryption
		{
		private:
			UInt8 key[32];
			UInt8 iv[12];
		public:
			AES256GCM(UnsafeArray<const UInt8> key, UnsafeArray<const UInt8> iv);
			virtual ~AES256GCM();

			virtual UIntOS Encrypt(UnsafeArray<const UInt8> inBuff, UIntOS inSize, UnsafeArray<UInt8> outBuff);
			virtual UIntOS Decrypt(UnsafeArray<const UInt8> inBuff, UIntOS inSize, UnsafeArray<UInt8> outBuff);

			virtual UIntOS GetEncBlockSize() const;
			virtual UIntOS GetDecBlockSize() const;

			void SetIV(UnsafeArray<const UInt8> iv); //12 bytes
		};
	}
}
#endif
