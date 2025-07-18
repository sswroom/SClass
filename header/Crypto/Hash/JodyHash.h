#ifndef _SM_CRYPTO_HASH_DJB2
#define _SM_CRYPTO_HASH_DJB2
#include "Crypto/Hash/HashAlgorithm.h"

namespace Crypto
{
	namespace Hash
	{
		class DJB2 : public HashAlgorithm
		{
		private:
			UInt32 currVal;

		public:
			DJB2();
			virtual ~DJB2();

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
