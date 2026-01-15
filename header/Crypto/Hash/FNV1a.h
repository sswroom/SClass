#ifndef _SM_CRYPTO_HASH_FNV1A
#define _SM_CRYPTO_HASH_FNV1A
#include "Crypto/Hash/HashAlgorithm.h"

namespace Crypto
{
	namespace Hash
	{
		class FNV1a : public HashAlgorithm
		{
		private:
			UInt32 currVal;

		public:
			FNV1a();
			virtual ~FNV1a();

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
