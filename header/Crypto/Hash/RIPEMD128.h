#ifndef _SM_CRYPTO_HASH_RIPEMD128
#define _SM_CRYPTO_HASH_RIPEMD128
#include "Crypto/Hash/IHash.h"

namespace Crypto
{
	namespace Hash
	{
		class RIPEMD128 : public IHash
		{
		private:
			Int64 msgLeng;
			UInt32 h0;
			UInt32 h1;
			UInt32 h2;
			UInt32 h3;
			UInt8 *buff;
			UInt32 buffSize;

		public:
			RIPEMD128();
			virtual ~RIPEMD128();

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
