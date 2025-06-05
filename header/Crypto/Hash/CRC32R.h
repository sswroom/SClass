#ifndef _SM_CRYPTO_HASH_CRC32R
#define _SM_CRYPTO_HASH_CRC32R
#include "Crypto/Hash/CRC32R_C.h"
#include "Crypto/Hash/HashAlgorithm.h"

namespace Crypto
{
	namespace Hash
	{
		class CRC32R : public HashAlgorithm
		{
		private:
			UnsafeArray<UInt32> crctab;
			UInt32 currVal;

		private:
			CRC32R(NN<const CRC32R> crc);
			void InitTable(UInt32 polynomial);
		public:
			CRC32R(); //IEEE polynomial
			CRC32R(UInt32 polynomial);
			virtual ~CRC32R();

			virtual UnsafeArray<UTF8Char> GetName(UnsafeArray<UTF8Char> sbuff) const;
			virtual NN<HashAlgorithm> Clone() const;
			virtual void Clear();
			virtual void Calc(UnsafeArray<const UInt8> buff, UOSInt buffSize);
			virtual void GetValue(UnsafeArray<UInt8> buff) const;
			virtual UOSInt GetBlockSize() const;
			virtual UOSInt GetResultSize() const;

			UInt32 CalcDirect(UnsafeArray<const UInt8> buff, UOSInt buffSize) const
			{
				return ~CRC32R_Calc(buff.Ptr(), buffSize, this->crctab.Ptr(), 0xffffffff);
			}
		};
	}
}
#endif
