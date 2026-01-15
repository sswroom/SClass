#ifndef _SM_CRYPTO_HASH_CRC16R
#define _SM_CRYPTO_HASH_CRC16R
#include "Crypto/Hash/CRC16R_C.h"
#include "Crypto/Hash/HashAlgorithm.h"

namespace Crypto
{
	namespace Hash
	{
		class CRC16R : public HashAlgorithm
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
			virtual NN<HashAlgorithm> Clone() const;
			virtual void Clear();
			virtual void Calc(UnsafeArray<const UInt8> buff, UIntOS buffSize);
			virtual void GetValue(UnsafeArray<UInt8> buff) const;
			virtual UIntOS GetBlockSize() const;
			virtual UIntOS GetResultSize() const;

			UInt16 Reverse(UInt16 polynomial);

			UInt16 CalcDirect(UnsafeArray<const UInt8> buff, UIntOS buffSize)
			{
				return (UInt16)~CRC16R_Calc(buff.Ptr(), buffSize, this->crctab.Ptr(), 0xffff);
			}
		};
	}
}
#endif
