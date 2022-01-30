#ifndef _SM_CRYPTO_HASH_SHA384
#define _SM_CRYPTO_HASH_SHA384
#include "Crypto/Hash/IHash.h"

namespace Crypto
{
	namespace Hash
	{
		class SHA384 : public Crypto::Hash::IHash
		{
		private:
			UInt64 intermediateHash[8];
			UInt64 messageLength;
			UOSInt messageBlockIndex;
			UInt8 messageBlock[128];

		public:
			SHA384();
			virtual ~SHA384();

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
