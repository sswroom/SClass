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
			NN<Crypto::Hash::IHash> hashInner;
			NN<Crypto::Hash::IHash> hashOuter;
			UInt8 *key;
			UOSInt keySize;

			UInt8 *iPad;
			UInt8 *oPad;
			UOSInt padSize;

		public:
			HMAC(NN<Crypto::Hash::IHash> hash, UnsafeArray<const UInt8> key, UOSInt keySize);
			virtual ~HMAC();

			virtual UnsafeArray<UTF8Char> GetName(UnsafeArray<UTF8Char> sbuff) const;
			virtual NN<IHash> Clone() const;
			virtual void Clear();
			virtual void Calc(const UInt8 *buff, UOSInt buffSize);
			virtual void GetValue(UnsafeArray<UInt8> buff) const;
			virtual UOSInt GetBlockSize() const;
			virtual UOSInt GetResultSize() const;
		};
	}
}
#endif
