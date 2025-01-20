#ifndef _SM_CRYPTO_HASH_SHA1_SHA1
#define _SM_CRYPTO_HASH_SHA1_SHA1
#include "Crypto/Hash/SHA1.h"

namespace Crypto
{
	namespace Hash
	{
		class SHA1_SHA1 : public Crypto::Hash::HashAlgorithm
		{
		private:
			NN<Crypto::Hash::SHA1> sha1;

			SHA1_SHA1(NN<Crypto::Hash::SHA1> sha1);
		public:
			SHA1_SHA1();
			virtual ~SHA1_SHA1();

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
