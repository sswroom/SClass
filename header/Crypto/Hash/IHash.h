#ifndef _SM_CRYPTO_HASH_IHASH
#define _SM_CRYPTO_HASH_IHASH
namespace Crypto
{
	namespace Hash
	{
		typedef enum
		{
			HT_UNKNOWN,
			// Primary Algorithm
			HT_ADLER32,
			HT_CRC16,
			HT_CRC16R,
			HT_CRC32,
			HT_CRC32R_IEEE,
			HT_DJB2,
			HT_DJB2A,
			HT_FNV1,
			HT_FNV1A,
			HT_MD5,
			HT_RIPEMD128,
			HT_RIPEMD160,
			HT_SDBM,
			HT_SHA1,
			HT_EXCEL,
			HT_SHA224,
			HT_SHA256,
			HT_SHA384,
			HT_SHA512,

			// Compound Algorithm
			HT_SHA1_SHA1,

			HT_FIRST = HT_ADLER32,
			HT_LAST = HT_SHA1_SHA1
		} HashType;

		class IHash
		{
		public:
			virtual ~IHash(){};

			virtual UTF8Char *GetName(UTF8Char *sbuff) = 0;
			virtual IHash *Clone() = 0;
			virtual void Clear() = 0;
			virtual void Calc(const UInt8 *buff, UOSInt buffSize) = 0;
			virtual void GetValue(UInt8 *buff) = 0;
			virtual UOSInt GetBlockSize() = 0;
			virtual UOSInt GetResultSize() = 0;
		};
	}
}
#endif
