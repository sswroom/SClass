#ifndef _SM_CRYPTO_HASH_SHA224
#define _SM_CRYPTO_HASH_SHA224
#include "Crypto/Hash/IHash.h"

namespace Crypto
{
	namespace Hash
	{
		class SHA224 : public Crypto::Hash::IHash
		{
		private:
			UInt32 intermediateHash[8];
			UInt64 messageLength;
			UOSInt messageBlockIndex;
			UInt8 messageBlock[64];

		public:
			SHA224();
			virtual ~SHA224();

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
