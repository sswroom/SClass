#ifndef _SM_CRYPTO_HASH_HASHCALC
#define _SM_CRYPTO_HASH_HASHCALC
#include "Crypto/Hash/HashAlgorithm.h"
#include "Sync/Mutex.h"
#include "Text/CString.h"

namespace Crypto
{
	namespace Hash
	{
		class HashCalc
		{
		private:
			NN<Crypto::Hash::HashAlgorithm> hash;
			Sync::Mutex mut;

		public:
			HashCalc(NN<Crypto::Hash::HashAlgorithm> hash); //hash will be released
			~HashCalc();

			void Calc(UnsafeArray<const UInt8> buff, UOSInt size, UnsafeArray<UInt8> hashVal);
			void CalcStr(Text::CStringNN s, UnsafeArray<UInt8> hashVal);
		};
	}
}
#endif
