#include "Stdafx.h"
#include "Crypto/Hash/IHash.h"

Text::CString Crypto::Hash::HashTypeGetName(HashType hashType)
{
	switch (hashType)
	{
	case HT_ADLER32:
		return CSTR("Adler-32");
	case HT_CRC16:
		return CSTR("CRC-16");
	case HT_CRC16R:
		return CSTR("CRC-16 (Reversed)");
	case HT_CRC32:
		return CSTR("CRC-32");
	case HT_CRC32R_IEEE:
		return CSTR("CRC-32 (IEEE)");
	case HT_CRC32C:
		return CSTR("CRC-32 (CCITT)");
	case HT_DJB2:
		return CSTR("DJB2");
	case HT_DJB2A:
		return CSTR("DJB2a");
	case HT_FNV1:
		return CSTR("FNV1");
	case HT_FNV1A:
		return CSTR("FNV1a");
	case HT_MD5:
		return CSTR("MD5");
	case HT_RIPEMD128:
		return CSTR("RIPEMD-128");
	case HT_RIPEMD160:
		return CSTR("RIPEMD-160");
	case HT_SDBM:
		return CSTR("SDBM");
	case HT_SHA1:
		return CSTR("SHA-1");
	case HT_EXCEL:
		return CSTR("Excel Hash");
	case HT_SHA224:
		return CSTR("SHA-224");
	case HT_SHA256:
		return CSTR("SHA-256");
	case HT_SHA384:
		return CSTR("SHA-384");
	case HT_SHA512:
		return CSTR("SHA-512");
	case HT_SHA1_SHA1:
		return CSTR("SHA-1 SHA-1");
	default:
	case HT_UNKNOWN:
		return CSTR("Unknown");
	}
}
