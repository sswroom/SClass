#ifndef _SM_CRYPTO_HASH_CRC32
#define _SM_CRYPTO_HASH_CRC32
#include "Crypto/Hash/IHash.h"

namespace Crypto
{
	namespace Hash
	{
		class CRC32 : public IHash
		{
		private:
			UInt32 *crctab;
			UInt32 currVal;

		private:
			CRC32(const CRC32 *crc);
			void InitTable(UInt32 polynomial);
		public:
			CRC32(); //IEEE polynomial
			CRC32(UInt32 polynomial);
			virtual ~CRC32();

			virtual UTF8Char *GetName(UTF8Char *sbuff);
			virtual IHash *Clone();
			virtual void Clear();
			virtual void Calc(const UInt8 *buff, UOSInt buffSize);
			virtual void GetValue(UInt8 *buff);
			virtual UOSInt GetBlockSize();
			virtual UOSInt GetResultSize();

			static UInt32 GetPolynormialIEEE();
			static UInt32 GetPolynormialCastagnoli();
		};
	}
}
#endif
