#ifndef _SM_CRYPTO_HASH_BSDCHKSUM
#define _SM_CRYPTO_HASH_BSDCHKSUM
#include "Crypto/Hash/IHash.h"

namespace Crypto
{
	namespace Hash
	{
		class BSDChkSum : public IHash
		{
		private:
			UInt16 chksum;

		private:
			BSDChkSum(const BSDChkSum *adler32);
		public:
			BSDChkSum();
			virtual ~BSDChkSum();

			virtual IHash *Clone();
			virtual void Clear();
			virtual void Calc(const UInt8 *buff, UOSInt buffSize);
			virtual void GetValue(UInt8 *buff);
			virtual UOSInt GetBlockSize();
			virtual UOSInt GetResultSize();
		};
	}
}
#endif
