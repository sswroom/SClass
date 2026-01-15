#ifndef _SM_CRYPTO_HASH_FNV1
#define _SM_CRYPTO_HASH_FNV1
#include "Crypto/Hash/HashAlgorithm.h"

namespace Crypto
{
	namespace Hash
	{
		class FNV1 : public HashAlgorithm
		{
		private:
			UInt32 currVal;

		public:
			FNV1();
			virtual ~FNV1();

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
