#ifndef _SM_CRYPTO_ENCRYPT_XORENC
#define _SM_CRYPTO_ENCRYPT_XORENC

#include "Crypto/Encrypt/ICrypto.h"

namespace Crypto
{
	namespace Encrypt
	{
		class XOREnc : public Crypto::Encrypt::ICrypto
		{
		public:
			XOREnc();
			virtual ~XOREnc();

			virtual UOSInt Encrypt(UnsafeArray<const UInt8> inBuff, UOSInt inSize, UnsafeArray<UInt8> outBuff);
			virtual UOSInt Decrypt(UnsafeArray<const UInt8> inBuff, UOSInt inSize, UnsafeArray<UInt8> outBuff);

			virtual UOSInt GetEncBlockSize() const;
			virtual UOSInt GetDecBlockSize() const;
		};
	}
}

#endif
