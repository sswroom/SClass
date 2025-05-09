#ifndef _SM_CRYPTO_HASH_DJB2A
#define _SM_CRYPTO_HASH_DJB2A
#include "Crypto/Hash/HashAlgorithm.h"

namespace Crypto
{
	namespace Hash
	{
		class DJB2a : public HashAlgorithm
		{
		private:
			UInt32 currVal;

		public:
			DJB2a();
			virtual ~DJB2a();

			virtual UnsafeArray<UTF8Char> GetName(UnsafeArray<UTF8Char> sbuff) const;
			virtual NN<HashAlgorithm> Clone() const;
			virtual void Clear();
			virtual void Calc(UnsafeArray<const UInt8> buff, UOSInt buffSize);
			virtual void GetValue(UnsafeArray<UInt8> buff) const;
			virtual UOSInt GetBlockSize() const;
			virtual UOSInt GetResultSize() const;
		};
	}
}
#endif
