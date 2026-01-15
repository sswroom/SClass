#ifndef _SM_CRYPTO_PBKDF2
#define _SM_CRYPTO_PBKDF2
#include "Crypto/Hash/HashAlgorithm.h"

namespace Crypto
{
	class PBKDF2
	{
	private:
		static UIntOS F(UnsafeArray<const UInt8> salt, UIntOS saltLen, UIntOS iterationCount, UInt32 i, NN<Crypto::Hash::HashAlgorithm> hashFunc, UnsafeArray<UInt8> outBuff);
	public:
		static UIntOS Calc(UnsafeArray<const UInt8> salt, UIntOS saltLen, UIntOS iterationCount, UIntOS dkLen, NN<Crypto::Hash::HashAlgorithm> hashFunc, UnsafeArray<UInt8> outBuff);
	};
}
#endif
