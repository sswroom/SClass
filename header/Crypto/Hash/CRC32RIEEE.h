#ifndef _SM_CRYPTO_HASH_CRC32RIEEE
#define _SM_CRYPTO_HASH_CRC32RIEEE
#include "Crypto/Hash/IHash.h"

extern "C"
{
	UInt32 CRC32R_Reverse(UInt32 polynomial);
	UInt32 CRC32R_Calc(const UInt8 *buff, UOSInt buffSize, UInt32 *tab, UInt32 currVal);
}

namespace Crypto
{
	namespace Hash
	{
		class CRC32RIEEE : public IHash
		{
		private:
			static UInt32 crctab[4096];
			static Bool tabInited;
			UInt32 currVal;

		public:
			CRC32RIEEE();
			virtual ~CRC32RIEEE();

			virtual UTF8Char *GetName(UTF8Char *sbuff) const;
			virtual NotNullPtr<IHash> Clone() const;
			virtual void Clear();
			virtual void Calc(const UInt8 *buff, UOSInt buffSize);
			virtual void GetValue(UInt8 *buff) const;
			virtual UOSInt GetBlockSize() const;
			virtual UOSInt GetResultSize() const;
			UInt32 GetValueU32() const;

			UInt32 CalcDirect(const UInt8 *buff, UOSInt buffSize) const
			{
				return ~CRC32R_Calc(buff, buffSize, this->crctab, 0xffffffff);
			}
		};
	}
}
#endif
