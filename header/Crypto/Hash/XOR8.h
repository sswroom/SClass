#ifndef _SM_CRYPTO_HASH_XOR8
#define _SM_CRYPTO_HASH_XOR8
#include "Crypto/Hash/IHash.h"

namespace Crypto
{
	namespace Hash
	{
		class XOR8 : public IHash
		{
		private:
			UInt8 val;

		private:
			XOR8(const XOR8 *xor8);
		public:
			XOR8();
			virtual ~XOR8();

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
