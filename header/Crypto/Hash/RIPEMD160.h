#ifndef _SM_CRYPTO_HASH_RIPEMD160
#define _SM_CRYPTO_HASH_RIPEMD160
#include "Crypto/Hash/HashAlgorithm.h"

namespace Crypto
{
	namespace Hash
	{
		class RIPEMD160 : public HashAlgorithm
		{
		private:
			UInt64 msgLeng;
			UInt32 h0;
			UInt32 h1;
			UInt32 h2;
			UInt32 h3;
			UInt32 h4;
			UInt8 buff[64];
			UInt32 buffSize;

		public:
			RIPEMD160();
			virtual ~RIPEMD160();

			virtual UnsafeArray<UTF8Char> GetName(UnsafeArray<UTF8Char> sbuff) const;
			virtual NN<HashAlgorithm> Clone() const;
			virtual void Clear();
			virtual void Calc(UnsafeArray<const UInt8> buff, UIntOS buffSize);
			virtual void GetValue(UnsafeArray<UInt8> buff) const; //20 bytes
			virtual UIntOS GetBlockSize() const;
			virtual UIntOS GetResultSize() const;
		};
	}
}

#endif
