#ifndef _SM_CRYPTO_HASH_FNV1A
#define _SM_CRYPTO_HASH_FNV1A
#include "Crypto/Hash/IHash.h"

namespace Crypto
{
	namespace Hash
	{
		class FNV1a : public IHash
		{
		private:
			UInt32 currVal;

		public:
			FNV1a();
			virtual ~FNV1a();

			virtual UTF8Char *GetName(UTF8Char *sbuff) const;
			virtual NotNullPtr<IHash> Clone() const;
			virtual void Clear();
			virtual void Calc(const UInt8 *buff, UOSInt buffSize);
			virtual void GetValue(UInt8 *buff) const;
			virtual UOSInt GetBlockSize() const;
			virtual UOSInt GetResultSize() const;
		};
	}
}
#endif
