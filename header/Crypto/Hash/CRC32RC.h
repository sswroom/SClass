#ifndef _SM_CRYPTO_HASH_CRC32RC
#define _SM_CRYPTO_HASH_CRC32RC
#include "Crypto/Hash/IHash.h"

extern "C"
{
	UInt32 CRC32R_Reverse(UInt32 polynomial);
}

namespace Crypto
{
	namespace Hash
	{
		class CRC32RC : public IHash
		{
		private:
			static UInt32 crctab[4096];
			static Bool tabInited;
			UInt32 currVal;

		public:
			CRC32RC();
			virtual ~CRC32RC();

			virtual UTF8Char *GetName(UTF8Char *sbuff);
			virtual IHash *Clone();
			virtual void Clear();
			virtual void Calc(const UInt8 *buff, UOSInt buffSize);
			virtual void GetValue(UInt8 *buff);
			virtual UOSInt GetBlockSize();
			virtual UOSInt GetResultSize();
		};
	}
}
#endif