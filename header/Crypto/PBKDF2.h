#ifndef _SM_CRYPTO_PBKDF2
#define _SM_CRYPTO_PBKDF2
#include "Crypto/Hash/HashAlgorithm.h"

namespace Crypto
{
	class PBKDF2
	{
	private:
		static UOSInt F(UnsafeArray<const UInt8> salt, UOSInt saltLen, UOSInt iterationCount, UInt32 i, NN<Crypto::Hash::HashAlgorithm> hashFunc, UnsafeArray<UInt8> outBuff);
	public:
		static UOSInt Calc(UnsafeArray<const UInt8> salt, UOSInt saltLen, UOSInt iterationCount, UOSInt dkLen, NN<Crypto::Hash::HashAlgorithm> hashFunc, UnsafeArray<UInt8> outBuff);
	};
}
#endif
