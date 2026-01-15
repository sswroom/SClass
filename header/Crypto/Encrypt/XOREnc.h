#ifndef _SM_CRYPTO_ENCRYPT_XORENC
#define _SM_CRYPTO_ENCRYPT_XORENC

#include "Crypto/Encrypt/Encryption.h"

namespace Crypto
{
	namespace Encrypt
	{
		class XOREnc : public Crypto::Encrypt::Encryption
		{
		public:
			XOREnc();
			virtual ~XOREnc();

			virtual UIntOS Encrypt(UnsafeArray<const UInt8> inBuff, UIntOS inSize, UnsafeArray<UInt8> outBuff);
			virtual UIntOS Decrypt(UnsafeArray<const UInt8> inBuff, UIntOS inSize, UnsafeArray<UInt8> outBuff);

			virtual UIntOS GetEncBlockSize() const;
			virtual UIntOS GetDecBlockSize() const;
		};
	}
}

#endif
