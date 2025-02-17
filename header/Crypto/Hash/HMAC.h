#ifndef _SM_CRYPTO_HASH_HMAC
#define _SM_CRYPTO_HASH_HMAC
#include "Crypto/Hash/HashAlgorithm.h"

namespace Crypto
{
	namespace Hash
	{
		class HMAC : public HashAlgorithm
		{
		private:
			NN<Crypto::Hash::HashAlgorithm> hashInner;
			NN<Crypto::Hash::HashAlgorithm> hashOuter;
			UnsafeArray<UInt8> key;
			UOSInt keySize;

			UnsafeArray<UInt8> iPad;
			UnsafeArray<UInt8> oPad;
			UOSInt padSize;

		public:
			HMAC(NN<Crypto::Hash::HashAlgorithm> hash, UnsafeArray<const UInt8> key, UOSInt keySize);
			virtual ~HMAC();

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
