#ifndef _SM_CRYPTO_HASH_SHA1
#define _SM_CRYPTO_HASH_SHA1
#include "Crypto/Hash/HashAlgorithm.h"

namespace Crypto
{
	namespace Hash
	{
		class SHA1 : public Crypto::Hash::HashAlgorithm
		{
		private:
			UInt32 intermediateHash[5];
			UInt64 messageLength;
			UIntOS messageBlockIndex;
			UInt8 messageBlock[64];

		public:
			SHA1();
			virtual ~SHA1();

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
