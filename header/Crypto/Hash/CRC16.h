#ifndef _SM_CRYPTO_HASH_CRC16
#define _SM_CRYPTO_HASH_CRC16
#include "Crypto/Hash/IHash.h"

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

			virtual UTF8Char *GetName(UTF8Char *sbuff);
			virtual IHash *Clone();
			virtual void Clear();
			virtual void Calc(const UInt8 *buff, UOSInt buffSize);
			virtual void GetValue(UInt8 *buff);
			virtual UOSInt GetBlockSize();
			virtual UOSInt GetResultSize();

			void SetValue(UInt16 val);

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
