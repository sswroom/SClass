#ifndef _SM_CRYPTO_ENCRYPT_RC4CIPHER
#define _SM_CRYPTO_ENCRYPT_RC4CIPHER
#include "Crypto/Encrypt/Encryption.h"

namespace Crypto
{
	namespace Encrypt
	{
		class RC4Cipher : public Encryption
		{
		private:
			UInt8 s[256];
			UInt8 i;
			UInt8 j;

		public:
			RC4Cipher(UnsafeArray<const UInt8> key, UIntOS keyLen);
			virtual ~RC4Cipher();

			void SetKey(UnsafeArray<const UInt8> key, UIntOS keyLen);

			virtual UIntOS Encrypt(UnsafeArray<const UInt8> inBuff, UIntOS inSize, UnsafeArray<UInt8> outBuff); //outBuff = null to get the size
			virtual UIntOS Decrypt(UnsafeArray<const UInt8> inBuff, UIntOS inSize, UnsafeArray<UInt8> outBuff); //outBuff = null to get the size

			virtual UIntOS GetEncBlockSize() const;
			virtual UIntOS GetDecBlockSize() const;
		};
	}
}
#endif
