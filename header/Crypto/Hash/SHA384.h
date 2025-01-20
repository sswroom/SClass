#ifndef _SM_CRYPTO_HASH_SHA384
#define _SM_CRYPTO_HASH_SHA384
#include "Crypto/Hash/HashAlgorithm.h"

namespace Crypto
{
	namespace Hash
	{
		class SHA384 : public Crypto::Hash::HashAlgorithm
		{
		private:
			UInt64 intermediateHash[8];
			UInt64 messageLength;
			UOSInt messageBlockIndex;
			UInt8 messageBlock[128];

		public:
			SHA384();
			virtual ~SHA384();

			virtual UnsafeArray<UTF8Char> GetName(UnsafeArray<UTF8Char> sbuff) const;
			virtual NN<HashAlgorithm> Clone() const;
			virtual void Clear();
			virtual void Calc(UnsafeArray<const UInt8> buff, UOSInt buffSize);
			virtual void GetValue(UnsafeArray<UInt8> buff) const;
			virtual UOSInt GetBlockSize() const;
			virtual UOSInt GetResultSize() const;
		};
	}
}
#endif
