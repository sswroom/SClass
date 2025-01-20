#ifndef _SM_CRYPTO_HASH_AESCMAC
#define _SM_CRYPTO_HASH_AESCMAC
#include "Crypto/Encrypt/AES128.h"
#include "Crypto/Hash/HashAlgorithm.h"

namespace Crypto
{
	namespace Hash
	{
		class AESCMAC : public HashAlgorithm
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

			virtual UnsafeArray<UTF8Char> GetName(UnsafeArray<UTF8Char> sbuff) const;
			virtual NN<HashAlgorithm> Clone() const;
			virtual void Clear();
			virtual void Calc(UnsafeArray<const UInt8> buff, UOSInt buffSize);
			virtual void GetValue(UnsafeArray<UInt8> buff) const;
			virtual UOSInt GetBlockSize() const;
			virtual UOSInt GetResultSize() const;
		};
	}
}
#endif
