#ifndef _SM_CRYPTO_HASH_SHA512
#define _SM_CRYPTO_HASH_SHA512
#include "Crypto/Hash/HashAlgorithm.h"

namespace Crypto
{
	namespace Hash
	{
		class SHA512 : public Crypto::Hash::HashAlgorithm
		{
		private:
			UInt64 intermediateHash[8];
			UInt64 messageLength;
			UIntOS messageBlockIndex;
			UInt8 messageBlock[128];

		public:
			SHA512();
			virtual ~SHA512();

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
