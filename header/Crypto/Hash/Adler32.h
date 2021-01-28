#ifndef _SM_CRYPTO_HASH_ADLER32
#define _SM_CRYPTO_HASH_ADLER32
#include "Crypto/Hash/IHash.h"

namespace Crypto
{
	namespace Hash
	{
		class Adler32 : public IHash
		{
		private:
			UInt32 abVal;

		private:
			Adler32(const Adler32 *adler32);
		public:
			Adler32();
			virtual ~Adler32();

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
