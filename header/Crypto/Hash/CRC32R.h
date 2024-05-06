#ifndef _SM_CRYPTO_HASH_CRC32R
#define _SM_CRYPTO_HASH_CRC32R
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
		class CRC32R : public IHash
		{
		private:
			UInt32 *crctab;
			UInt32 currVal;

		private:
			CRC32R(const CRC32R *crc);
			void InitTable(UInt32 polynomial);
		public:
			CRC32R(); //IEEE polynomial
			CRC32R(UInt32 polynomial);
			virtual ~CRC32R();

			virtual UTF8Char *GetName(UTF8Char *sbuff) const;
			virtual NN<IHash> Clone() const;
			virtual void Clear();
			virtual void Calc(const UInt8 *buff, UOSInt buffSize);
			virtual void GetValue(UnsafeArray<UInt8> buff) const;
			virtual UOSInt GetBlockSize() const;
			virtual UOSInt GetResultSize() const;

			UInt32 CalcDirect(const UInt8 *buff, UOSInt buffSize) const
			{
				return ~CRC32R_Calc(buff, buffSize, this->crctab, 0xffffffff);
			}
		};
	}
}
#endif
