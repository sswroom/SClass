#ifndef _SM_CRYPTO_HASH_IHASH
#define _SM_CRYPTO_HASH_IHASH
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

		Text::CString HashTypeGetName(HashType hashType);

		class IHash
		{
		public:
			virtual ~IHash(){};

			virtual UTF8Char *GetName(UTF8Char *sbuff) const = 0;
			virtual IHash *Clone() const = 0;
			virtual void Clear() = 0;
			virtual void Calc(const UInt8 *buff, UOSInt buffSize) = 0;
			virtual void GetValue(UInt8 *buff) const = 0;
			virtual UOSInt GetBlockSize() const = 0;
			virtual UOSInt GetResultSize() const = 0;
		};
	}
}
#endif
