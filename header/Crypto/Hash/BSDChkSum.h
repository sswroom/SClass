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
			BSDChkSum(const BSDChkSum *adler32);
		public:
			BSDChkSum();
			virtual ~BSDChkSum();

			virtual HashAlgorithm *Clone() const;
			virtual void Clear();
			virtual void Calc(UnsafeArray<const UInt8> buff, UOSInt buffSize);
			virtual void GetValue(UInt8 *buff) const;
			virtual UOSInt GetBlockSize() const;
			virtual UOSInt GetResultSize() const;
		};
	}
}
#endif
