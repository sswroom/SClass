#ifndef _SM_CRYPTO_HASH_EXCELHASH
#define _SM_CRYPTO_HASH_EXCELHASH
#include "Crypto/Hash/IHash.h"

namespace Crypto
{
	namespace Hash
	{
		class ExcelHash : public IHash
		{
		private:
			UInt16 currVal;
			UInt16 charCnt;

		public:
			ExcelHash();
			virtual ~ExcelHash();

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
