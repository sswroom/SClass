#ifndef _SM_CRYPTO_HASH_CRC16
#define _SM_CRYPTO_HASH_CRC16
#include "Crypto/Hash/CRC16_C.h"
#include "Crypto/Hash/HashAlgorithm.h"

namespace Crypto
{
	namespace Hash
	{
		class CRC16 : public HashAlgorithm
		{
		private:
			UnsafeArray<UInt16> crctab;
			UInt16 currVal;
			UInt16 polynomial;

			void BuildTable(UInt16 polynomial);
		public:
			CRC16(UInt16 polynomial);
			CRC16(); //CCITT
			virtual ~CRC16();

			virtual UnsafeArray<UTF8Char> GetName(UnsafeArray<UTF8Char> sbuff) const;
			virtual NN<HashAlgorithm> Clone() const;
			virtual void Clear();
			virtual void Calc(UnsafeArray<const UInt8> buff, UOSInt buffSize);
			virtual void GetValue(UnsafeArray<UInt8> buff) const;
			virtual UOSInt GetBlockSize() const;
			virtual UOSInt GetResultSize() const;

			void SetValue(UInt16 val);

			UInt16 CalcDirect(UnsafeArray<const UInt8> buff, UOSInt buffSize)
			{
				return CRC16_Calc(buff.Ptr(), buffSize, this->crctab.Ptr(), 0);
			}

			static UInt16 GetPolynomialANSI();
			static UInt16 GetPolynomialCCITT();
			static UInt16 GetPolynomialT10_DIF();
			static UInt16 GetPolynomialDNP();
			static UInt16 GetPolynomialDECT();
			static UInt16 GetPolynomialARINC();
			static UInt16 GetPolynomialMODBUS();
		};
	}
}
#endif
