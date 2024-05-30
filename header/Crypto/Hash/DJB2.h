#ifndef _SM_CRYPTO_HASH_DJB2
#define _SM_CRYPTO_HASH_DJB2
#include "Crypto/Hash/IHash.h"

namespace Crypto
{
	namespace Hash
	{
		class DJB2 : public IHash
		{
		private:
			UInt32 currVal;

		public:
			DJB2();
			virtual ~DJB2();

			virtual UnsafeArray<UTF8Char> GetName(UnsafeArray<UTF8Char> sbuff) const;
			virtual NN<IHash> Clone() const;
			virtual void Clear();
			virtual void Calc(const UInt8 *buff, UOSInt buffSize);
			virtual void GetValue(UnsafeArray<UInt8> buff) const;
			virtual UOSInt GetBlockSize() const;
			virtual UOSInt GetResultSize() const;
		};
	}
}
#endif
