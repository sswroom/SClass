#ifndef _SM_CRYPTO_HASH_SUPERFASTHASH
#define _SM_CRYPTO_HASH_SUPERFASTHASH
#include "Crypto/Hash/HashAlgorithm.h"

namespace Crypto
{
	namespace Hash
	{
		class SuperFastHash : public HashAlgorithm
		{
		private:
			UInt32 currVal;

		public:
			SuperFastHash(UInt32 len);
			virtual ~SuperFastHash();

			virtual UnsafeArray<UTF8Char> GetName(UnsafeArray<UTF8Char> sbuff) const;
			virtual NN<HashAlgorithm> Clone() const;
			virtual void Clear();
			virtual void Calc(UnsafeArray<const UInt8> buff, UIntOS buffSize);
			virtual void GetValue(UnsafeArray<UInt8> buff) const;
			virtual UIntOS GetBlockSize() const;
			virtual UIntOS GetResultSize() const;
		};
	}
}
#endif
