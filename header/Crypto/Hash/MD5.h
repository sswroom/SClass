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
			UInt64 msgLeng;
			UInt32 h[4];
			UInt8 *buff;
			UOSInt buffSize;

		public:
			MD5();
			virtual ~MD5();

			virtual UTF8Char *GetName(UTF8Char *sbuff) const;
			virtual IHash *Clone() const;
			virtual void Clear();
			virtual void Calc(const UInt8 *buff, UOSInt buffSize);
			virtual void GetValue(UInt8 *buff) const; //16 bytes
			virtual UOSInt GetBlockSize() const;
			virtual UOSInt GetResultSize() const;
		};
	}
}

#endif
