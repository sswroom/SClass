#ifndef _SM_CRYPTO_ENCRYPT_BASE64
#define _SM_CRYPTO_ENCRYPT_BASE64
#include "Crypto/Encrypt/Encryption.h"

namespace Crypto
{
	namespace Encrypt
	{
		class Base64 : public ::Crypto::Encrypt::Encryption
		{
		public:
			Base64();
			virtual ~Base64();

			virtual UIntOS Encrypt(UnsafeArray<const UInt8> inBuff, UIntOS inSize, UnsafeArray<UInt8> outBuff);
			virtual UIntOS Decrypt(UnsafeArray<const UInt8> inBuff, UIntOS inSize, UnsafeArray<UInt8> outBuff);

			virtual UIntOS GetEncBlockSize() const;
			virtual UIntOS GetDecBlockSize() const;
		};
	}
}
#endif
