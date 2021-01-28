#ifndef _SM_CRYPTO_HASH_SDBM
#define _SM_CRYPTO_HASH_SDBM
#include "Crypto/Hash/IHash.h"

namespace Crypto
{
	namespace Hash
	{
		class SDBM : public IHash
		{
		private:
			UInt32 currVal;

		public:
			SDBM();
			virtual ~SDBM();

			virtual UTF8Char *GetName(UTF8Char *sbuff);
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
