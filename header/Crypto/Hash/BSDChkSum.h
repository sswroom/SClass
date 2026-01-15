#ifndef _SM_CRYPTO_HASH_BSDCHKSUM
#define _SM_CRYPTO_HASH_BSDCHKSUM
#include "Crypto/Hash/HashAlgorithm.h"

namespace Crypto
{
	namespace Hash
	{
		class BSDChkSum : public HashAlgorithm
		{
		private:
			UInt16 chksum;

		private:
			BSDChkSum(NN<const BSDChkSum> bsdchksum);
		public:
			BSDChkSum();
			virtual ~BSDChkSum();

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
