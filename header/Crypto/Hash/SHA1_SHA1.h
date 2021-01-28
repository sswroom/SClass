#ifndef _SM_CRYPTO_HASH_SHA1_SHA1
#define _SM_CRYPTO_HASH_SHA1_SHA1
#include "Crypto/Hash/SHA1.h"

namespace Crypto
{
	namespace Hash
	{
		class SHA1_SHA1 : public Crypto::Hash::IHash
		{
		private:
			Crypto::Hash::SHA1 *sha1;

			SHA1_SHA1(Crypto::Hash::SHA1 *sha1);
		public:
			SHA1_SHA1();
			virtual ~SHA1_SHA1();

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
