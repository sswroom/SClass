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

			virtual UnsafeArray<UTF8Char> GetName(UnsafeArray<UTF8Char> sbuff) const;
			virtual NN<IHash> Clone() const;
			virtual void Clear();
			virtual void Calc(UnsafeArray<const UInt8> buff, UOSInt buffSize);
			virtual void GetValue(UnsafeArray<UInt8> buff) const;
			virtual UOSInt GetBlockSize() const;
			virtual UOSInt GetResultSize() const;
		};
	}
}
#endif
