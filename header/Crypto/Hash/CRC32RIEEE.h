#ifndef _SM_CRYPTO_HASH_CRC32RIEEE
#define _SM_CRYPTO_HASH_CRC32RIEEE
#include "Crypto/Hash/IHash.h"

extern "C"
{
	UInt32 CRC32R_Reverse(UInt32 polynomial);
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