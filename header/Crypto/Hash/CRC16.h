#ifndef _SM_CRYPTO_HASH_CRC16
#define _SM_CRYPTO_HASH_CRC16
#include "Crypto/Hash/IHash.h"

extern "C"
{
	void CRC16_InitTable(UInt16 *tab, UInt16 polynomial);
	UInt16 CRC16_Calc(const UInt8 *buff, UOSInt buffSize, UInt16 *tab, UInt16 currVal);
}

namespace Crypto
{
	namespace Hash
	{
		class CRC16 : public IHash
		{
		private:
			UInt16 *crctab;
			UInt16 currVal;
			UInt16 polynomial;

			void BuildTable(UInt16 polynomial);
		public:
			CRC16(UInt16 polynomial);
			CRC16(); //CCITT
			virtual ~CRC16();

			virtual UTF8Char *GetName(UTF8Char *sbuff) const;
			virtual NN<IHash> Clone() const;
			virtual void Clear();
			virtual void Calc(const UInt8 *buff, UOSInt buffSize);
			virtual void GetValue(UInt8 *buff) const;
			virtual UOSInt GetBlockSize() const;
			virtual UOSInt GetResultSize() const;

			void SetValue(UInt16 val);

			UInt16 CalcDirect(const UInt8 *buff, UOSInt buffSize)
			{
				return CRC16_Calc(buff, buffSize, this->crctab, 0);
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
