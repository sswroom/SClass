#ifndef _SM_CRYPTO_HASH_CRC32
#define _SM_CRYPTO_HASH_CRC32
#include "Crypto/Hash/HashAlgorithm.h"

namespace Crypto
{
	namespace Hash
	{
		class CRC32 : public HashAlgorithm
		{
		private:
			UnsafeArray<UInt32> crctab;
			UInt32 currVal;

		private:
			CRC32(NN<const CRC32> crc);
			void InitTable(UInt32 polynomial);
		public:
			CRC32(); //IEEE polynomial
			CRC32(UInt32 polynomial);
			virtual ~CRC32();

			virtual UnsafeArray<UTF8Char> GetName(UnsafeArray<UTF8Char> sbuff) const;
			virtual NN<HashAlgorithm> Clone() const;
			virtual void Clear();
			virtual void Calc(UnsafeArray<const UInt8> buff, UOSInt buffSize);
			virtual void GetValue(UnsafeArray<UInt8> buff) const;
			virtual UOSInt GetBlockSize() const;
			virtual UOSInt GetResultSize() const;

			static UInt32 GetPolynormialIEEE();
			static UInt32 GetPolynormialCastagnoli();
		};
	}
}
#endif
