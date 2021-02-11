#ifndef _SM_CRYPTO_HASH_HASHCALC
#define _SM_CRYPTO_HASH_HASHCALC
#include "Crypto/Hash/IHash.h"
#include "Sync/Mutex.h"

namespace Crypto
{
	namespace Hash
	{
		class HashCalc
		{
		private:
			Crypto::Hash::IHash *hash;
			Sync::Mutex *mut;

		public:
			HashCalc(Crypto::Hash::IHash *hash); //hash will be released
			~HashCalc();

			void Calc(const UInt8 *buff, UOSInt size, UInt8 *hashVal);
			void CalcStr(const UTF8Char *s, UInt8 *hashVal);
		};
	}
}
#endif
