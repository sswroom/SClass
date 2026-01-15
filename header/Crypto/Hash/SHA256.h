#ifndef _SM_CRYPTO_HASH_SHA256
#define _SM_CRYPTO_HASH_SHA256
#include "Crypto/Hash/HashAlgorithm.h"

namespace Crypto
{
	namespace Hash
	{
		class SHA256 : public Crypto::Hash::HashAlgorithm
		{
		private:
			UInt32 intermediateHash[8];
			UInt64 messageLength;
			UIntOS messageBlockIndex;
			UInt8 messageBlock[64];

		public:
			SHA256();
			virtual ~SHA256();

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
