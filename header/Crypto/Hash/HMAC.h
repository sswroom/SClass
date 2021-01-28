#ifndef _SM_CRYPTO_HASH_HMAC
#define _SM_CRYPTO_HASH_HMAC
#include "Crypto/Hash/IHash.h"

namespace Crypto
{
	namespace Hash
	{
		class HMAC : public IHash
		{
		private:
			Crypto::Hash::IHash *hashInner;
			Crypto::Hash::IHash *hashOuter;
			UInt8 *key;
			UOSInt keySize;

			UInt8 *iPad;
			UInt8 *oPad;
			UOSInt padSize;

		public:
			HMAC(Crypto::Hash::IHash *hash, const UInt8 *key, UOSInt keySize);
			virtual ~HMAC();

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
