#ifndef _SM_CRYPTO_HASH_SHA256
#define _SM_CRYPTO_HASH_SHA256
#include "Crypto/Hash/IHash.h"

namespace Crypto
{
	namespace Hash
	{
		class SHA256 : public Crypto::Hash::IHash
		{
		private:
			UInt32 intermediateHash[8];
			UInt64 messageLength;
			UOSInt messageBlockIndex;
			UInt8 messageBlock[64];

		public:
			SHA256();
			virtual ~SHA256();

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
