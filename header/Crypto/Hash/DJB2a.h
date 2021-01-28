#ifndef _SM_CRYPTO_HASH_DJB2A
#define _SM_CRYPTO_HASH_DJB2A
#include "Crypto/Hash/IHash.h"

namespace Crypto
{
	namespace Hash
	{
		class DJB2a : public IHash
		{
		private:
			UInt32 currVal;

		public:
			DJB2a();
			virtual ~DJB2a();

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
