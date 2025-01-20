#ifndef _SM_CRYPTO_HASH_EXCELHASH
#define _SM_CRYPTO_HASH_EXCELHASH
#include "Crypto/Hash/HashAlgorithm.h"

namespace Crypto
{
	namespace Hash
	{
		class ExcelHash : public HashAlgorithm
		{
		private:
			UInt16 currVal;
			UInt16 charCnt;

		public:
			ExcelHash();
			virtual ~ExcelHash();

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
