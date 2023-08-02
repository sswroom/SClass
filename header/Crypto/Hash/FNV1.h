#ifndef _SM_CRYPTO_HASH_FNV1
#define _SM_CRYPTO_HASH_FNV1
#include "Crypto/Hash/IHash.h"

namespace Crypto
{
	namespace Hash
	{
		class FNV1 : public IHash
		{
		private:
			UInt32 currVal;

		public:
			FNV1();
			virtual ~FNV1();

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
