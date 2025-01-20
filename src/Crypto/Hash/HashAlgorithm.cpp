#include "Stdafx.h"
#include "Crypto/Hash/HashAlgorithm.h"

Text::CStringNN Crypto::Hash::HashTypeGetName(HashType hashType)
{
	switch (hashType)
	{
	case HashType::Adler32:
		return CSTR("Adler-32");
	case HashType::CRC16:
		return CSTR("CRC-16");
	case HashType::CRC16R:
		return CSTR("CRC-16 (Reversed)");
	case HashType::CRC32:
		return CSTR("CRC-32");
	case HashType::CRC32R_IEEE:
		return CSTR("CRC-32 (IEEE)");
	case HashType::CRC32C:
		return CSTR("CRC-32 (CCITT)");
	case HashType::DJB2:
		return CSTR("DJB2");
	case HashType::DJB2a:
		return CSTR("DJB2a");
	case HashType::FNV1:
		return CSTR("FNV1");
	case HashType::FNV1a:
		return CSTR("FNV1a");
	case HashType::MD5:
		return CSTR("MD5");
	case HashType::RIPEMD128:
		return CSTR("RIPEMD-128");
	case HashType::RIPEMD160:
		return CSTR("RIPEMD-160");
	case HashType::SDBM:
		return CSTR("SDBM");
	case HashType::SHA1:
		return CSTR("SHA-1");
	case HashType::Excel:
		return CSTR("Excel Hash");
	case HashType::SHA224:
		return CSTR("SHA-224");
	case HashType::SHA256:
		return CSTR("SHA-256");
	case HashType::SHA384:
		return CSTR("SHA-384");
	case HashType::SHA512:
		return CSTR("SHA-512");
	case HashType::MD4:
		return CSTR("MD4");
	case HashType::SHA1_SHA1:
		return CSTR("SHA-1 SHA-1");
	default:
	case HashType::Unknown:
		return CSTR("Unknown");
	}
}
