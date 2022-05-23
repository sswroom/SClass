#ifndef _SM_CRYPTO_HASH_CRC16R
#define _SM_CRYPTO_HASH_CRC16R
#include "Crypto/Hash/IHash.h"

extern "C"
{
	void CRC16R_InitTable(UInt16 *tab, UInt16 rpn);
	UInt16 CRC16R_Calc(const UInt8 *buff, UOSInt buffSize, UInt16 *tab, UInt16 currVal);
}

namespace Crypto
{
	namespace Hash
	{
		class CRC16R : public IHash
		{
		private:
			UInt16 *crctab;
			UInt16 currVal;
			UInt16 polynomial;

			void BuildTable(UInt16 polynomial);
		public:
			CRC16R(UInt16 polynomial);
			CRC16R(); //ANSI 0x8005
			virtual ~CRC16R();

			virtual UTF8Char *GetName(UTF8Char *sbuff);
			virtual IHash *Clone();
			virtual void Clear();
			virtual void Calc(const UInt8 *buff, UOSInt buffSize);
			virtual void GetValue(UInt8 *buff);
			virtual UOSInt GetBlockSize();
			virtual UOSInt GetResultSize();

			UInt16 Reverse(UInt16 polynomial);

			UInt16 CalcDirect(const UInt8 *buff, UOSInt buffSize)
			{
				return ~CRC16R_Calc(buff, buffSize, this->crctab, 0xffff);
			}
		};
	}
}
#endif
