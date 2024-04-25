#ifndef _SM_CRYPTO_HASH_SHA1
#define _SM_CRYPTO_HASH_SHA1
#include "Crypto/Hash/IHash.h"

namespace Crypto
{
	namespace Hash
	{
		class SHA1 : public Crypto::Hash::IHash
		{
		private:
			UInt32 intermediateHash[5];
			UInt64 messageLength;
			UOSInt messageBlockIndex;
			UInt8 messageBlock[64];

		public:
			SHA1();
			virtual ~SHA1();

			virtual UTF8Char *GetName(UTF8Char *sbuff) const;
			virtual NN<IHash> Clone() const;
			virtual void Clear();
			virtual void Calc(const UInt8 *buff, UOSInt buffSize);
			virtual void GetValue(UInt8 *buff) const;
			virtual UOSInt GetBlockSize() const;
			virtual UOSInt GetResultSize() const;
		};
	}
}
#endif
