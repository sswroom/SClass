#ifndef _SM_CRYPTO_HASH_XOR8
#define _SM_CRYPTO_HASH_XOR8
#include "Crypto/Hash/HashAlgorithm.h"

namespace Crypto
{
	namespace Hash
	{
		class XOR8 : public HashAlgorithm
		{
		private:
			UInt8 val;

		private:
			XOR8(NN<const XOR8> xor8);
		public:
			XOR8();
			virtual ~XOR8();

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
