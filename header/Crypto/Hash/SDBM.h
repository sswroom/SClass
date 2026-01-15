#ifndef _SM_CRYPTO_HASH_SDBM
#define _SM_CRYPTO_HASH_SDBM
#include "Crypto/Hash/HashAlgorithm.h"

namespace Crypto
{
	namespace Hash
	{
		class SDBM : public HashAlgorithm
		{
		private:
			UInt32 currVal;

		public:
			SDBM();
			virtual ~SDBM();

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
