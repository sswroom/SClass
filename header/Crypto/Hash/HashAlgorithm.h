#ifndef _SM_CRYPTO_HASH_HASHALGORITHM
#define _SM_CRYPTO_HASH_HASHALGORITHM
#include "Text/CString.h"

namespace Crypto
{
	namespace Hash
	{
		enum class HashType
		{
			Unknown,
			// Primary Algorithm
			Adler32,
			CRC16,
			CRC16R,
			CRC32,
			CRC32R_IEEE,
			CRC32C,
			DJB2,
			DJB2a,
			FNV1,
			FNV1a,
			MD5,
			RIPEMD128,
			RIPEMD160,
			SDBM,
			SHA1,
			Excel,
			SHA224,
			SHA256,
			SHA384,
			SHA512,
			MD4,

			// Compound Algorithm
			SHA1_SHA1,

			First = Adler32,
			Last = SHA1_SHA1
		};

		Text::CStringNN HashTypeGetName(HashType hashType);

		class HashAlgorithm
		{
		public:
			virtual ~HashAlgorithm(){};

			virtual UnsafeArray<UTF8Char> GetName(UnsafeArray<UTF8Char> sbuff) const = 0;
			virtual NN<HashAlgorithm> Clone() const = 0;
			virtual void Clear() = 0;
			virtual void Calc(UnsafeArray<const UInt8> buff, UIntOS buffSize) = 0;
			virtual void GetValue(UnsafeArray<UInt8> buff) const = 0;
			virtual UIntOS GetBlockSize() const = 0;
			virtual UIntOS GetResultSize() const = 0;
		};
	}
}
#endif
