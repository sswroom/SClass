#ifndef _SM_CRYPTO_HASH_SUPERFASTHASH
#define _SM_CRYPTO_HASH_SUPERFASTHASH
#include "Crypto/Hash/IHash.h"

namespace Crypto
{
	namespace Hash
	{
		class SuperFastHash : public IHash
		{
		private:
			UInt32 currVal;

		public:
			SuperFastHash(UInt32 len);
			virtual ~SuperFastHash();

			virtual UTF8Char *GetName(UTF8Char *sbuff);
			virtual IHash *Clone();
			virtual void Clear();
			virtual void Calc(const UInt8 *buff, UOSInt buffSize);
			virtual void GetValue(UInt8 *buff);
			virtual UOSInt GetBlockSize();
			virtual UOSInt GetResultSize();
		};
	}
}
#endif
