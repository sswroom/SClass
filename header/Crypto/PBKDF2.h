#ifndef _SM_CRYPTO_PBKDF2
#define _SM_CRYPTO_PBKDF2
#include "Crypto/Hash/IHash.h"

namespace Crypto
{
	class PBKDF2
	{
	private:
		static UOSInt F(const UInt8 *salt, UOSInt saltLen, UOSInt iterationCount, UInt32 i, Crypto::Hash::IHash *hashFunc, UInt8 *outBuff);
	public:
		static UOSInt Calc(const UInt8 *salt, UOSInt saltLen, UOSInt iterationCount, UOSInt dkLen, Crypto::Hash::IHash *hashFunc, UInt8 *outBuff);
	};
}
#endif
