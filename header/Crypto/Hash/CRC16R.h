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
			UnsafeArray<UInt16> crctab;
			UInt16 currVal;
			UInt16 polynomial;

			void BuildTable(UInt16 polynomial);
		public:
			CRC16R(UInt16 polynomial);
			CRC16R(); //ANSI 0x8005
			virtual ~CRC16R();

			virtual UnsafeArray<UTF8Char> GetName(UnsafeArray<UTF8Char> sbuff) const;
			virtual NN<IHash> Clone() const;
			virtual void Clear();
			virtual void Calc(UnsafeArray<const UInt8> buff, UOSInt buffSize);
			virtual void GetValue(UnsafeArray<UInt8> buff) const;
			virtual UOSInt GetBlockSize() const;
			virtual UOSInt GetResultSize() const;

			UInt16 Reverse(UInt16 polynomial);

			UInt16 CalcDirect(UnsafeArray<const UInt8> buff, UOSInt buffSize)
			{
				return (UInt16)~CRC16R_Calc(buff.Ptr(), buffSize, this->crctab.Ptr(), 0xffff);
			}
		};
	}
}
#endif
