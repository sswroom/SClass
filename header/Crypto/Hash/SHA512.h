#ifndef _SM_CRYPTO_HASH_SHA512
#define _SM_CRYPTO_HASH_SHA512
#include "Crypto/Hash/IHash.h"

namespace Crypto
{
	namespace Hash
	{
		class SHA512 : public Crypto::Hash::IHash
		{
		private:
			UInt64 intermediateHash[8];
			UInt64 messageLength;
			UOSInt messageBlockIndex;
			UInt8 messageBlock[128];

		public:
			SHA512();
			virtual ~SHA512();

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
