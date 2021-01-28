#ifndef _SM_CRYPTO_HASH_MD5
#define _SM_CRYPTO_HASH_MD5
#include "Crypto/Hash/IHash.h"

namespace Crypto
{
	namespace Hash
	{
		class MD5 : public IHash
		{
		private:
			Int64 msgLeng;
			UInt32 h[4];
			UInt8 *buff;
			UOSInt buffSize;

		public:
			MD5();
			virtual ~MD5();

			virtual UTF8Char *GetName(UTF8Char *sbuff);
			virtual IHash *Clone();
			virtual void Clear();
			virtual void Calc(const UInt8 *buff, UOSInt buffSize);
			virtual void GetValue(UInt8 *buff); //16 bytes
			virtual UOSInt GetBlockSize();
			virtual UOSInt GetResultSize();
		};
	}
}

#endif
