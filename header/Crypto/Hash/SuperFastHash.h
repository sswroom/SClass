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

			virtual UnsafeArray<UTF8Char> GetName(UnsafeArray<UTF8Char> sbuff) const;
			virtual NN<IHash> Clone() const;
			virtual void Clear();
			virtual void Calc(const UInt8 *buff, UOSInt buffSize);
			virtual void GetValue(UnsafeArray<UInt8> buff) const;
			virtual UOSInt GetBlockSize() const;
			virtual UOSInt GetResultSize() const;
		};
	}
}
#endif
