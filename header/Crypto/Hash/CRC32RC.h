#ifndef _SM_CRYPTO_HASH_CRC32RC
#define _SM_CRYPTO_HASH_CRC32RC
#include "Crypto/Hash/CRC32R_C.h"
#include "Crypto/Hash/HashAlgorithm.h"

namespace Crypto
{
	namespace Hash
	{
		class CRC32RC : public HashAlgorithm
		{
		private:
			static UInt32 crctab[4096];
			static Bool tabInited;
			UInt32 currVal;

		public:
			CRC32RC();
			virtual ~CRC32RC();

			virtual UnsafeArray<UTF8Char> GetName(UnsafeArray<UTF8Char> sbuff) const;
			virtual NN<HashAlgorithm> Clone() const;
			virtual void Clear();
			virtual void Calc(UnsafeArray<const UInt8> buff, UOSInt buffSize);
			virtual void GetValue(UnsafeArray<UInt8> buff) const;
			virtual UOSInt GetBlockSize() const;
			virtual UOSInt GetResultSize() const;
			UInt32 GetValueU32() const;

			UInt32 CalcDirect(UnsafeArray<const UInt8> buff, UOSInt buffSize) const
			{
				return ~CRC32R_Calc(buff.Ptr(), buffSize, this->crctab, 0xffffffff);
			}
		};
	}
}
#endif
