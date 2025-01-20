#ifndef _SM_CRYPTO_HASH_RIPEMD128
#define _SM_CRYPTO_HASH_RIPEMD128
#include "Crypto/Hash/HashAlgorithm.h"

namespace Crypto
{
	namespace Hash
	{
		class RIPEMD128 : public HashAlgorithm
		{
		private:
			UInt64 msgLeng;
			UInt32 h0;
			UInt32 h1;
			UInt32 h2;
			UInt32 h3;
			UInt8 buff[64];
			UInt32 buffSize;

		public:
			RIPEMD128();
			virtual ~RIPEMD128();

			virtual UnsafeArray<UTF8Char> GetName(UnsafeArray<UTF8Char> sbuff) const;
			virtual NN<HashAlgorithm> Clone() const;
			virtual void Clear();
			virtual void Calc(UnsafeArray<const UInt8> buff, UOSInt buffSize);
			virtual void GetValue(UnsafeArray<UInt8> buff) const; //16 bytes
			virtual UOSInt GetBlockSize() const;
			virtual UOSInt GetResultSize() const;
		};
	}
}

#endif
