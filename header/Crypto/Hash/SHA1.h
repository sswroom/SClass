#ifndef _SM_CRYPTO_HASH_SHA1
#define _SM_CRYPTO_HASH_SHA1
#include "Crypto/Hash/IHash.h"

namespace Crypto
{
	namespace Hash
	{
		class SHA1 : public Crypto::Hash::IHash
		{
		private:
			UInt32 *Intermediate_Hash;
			Int64 Message_Length;
			OSInt Message_Block_Index;
			UInt8 *Message_Block;

		public:
			SHA1();
			virtual ~SHA1();

			virtual UTF8Char *GetName(UTF8Char *sbuff);
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
