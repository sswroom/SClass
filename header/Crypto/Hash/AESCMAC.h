#ifndef _SM_CRYPTO_HASH_AESCMAC
#define _SM_CRYPTO_HASH_AESCMAC
#include "Crypto/Encrypt/AES128.h"
#include "Crypto/Hash/IHash.h"

namespace Crypto
{
	namespace Hash
	{
		class AESCMAC : public IHash
		{
		private:
			UInt8 key[16];
			UInt8 k1[16];
			UInt8 k2[16];
			Crypto::Encrypt::AES128 aes;

			UInt8 x[16];
			UInt8 buff[16];
			UOSInt buffSize;

			void GenSubKey();
		public:
			AESCMAC(const UInt8 *key);
			virtual ~AESCMAC();

			virtual UTF8Char *GetName(UTF8Char *sbuff) const;
			virtual NotNullPtr<IHash> Clone() const;
			virtual void Clear();
			virtual void Calc(const UInt8 *buff, UOSInt buffSize);
			virtual void GetValue(UInt8 *buff) const;
			virtual UOSInt GetBlockSize() const;
			virtual UOSInt GetResultSize() const;
		};
	}
}
#endif
