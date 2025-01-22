#ifndef _SM_CRYPTO_HASH_ADLER32
#define _SM_CRYPTO_HASH_ADLER32
#include "Crypto/Hash/HashAlgorithm.h"

extern "C"
{
	UInt32 Adler32_Calc(const UInt8 *buff, UOSInt buffSize, UInt32 abVal);
}

namespace Crypto
{
	namespace Hash
	{
		class Adler32 : public HashAlgorithm
		{
		private:
			UInt32 abVal;

		private:
			Adler32(const Adler32 *adler32);
		public:
			Adler32();
			virtual ~Adler32();

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
