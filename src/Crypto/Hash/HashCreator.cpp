#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/Adler32.h"
#include "Crypto/Hash/CRC16.h"
#include "Crypto/Hash/CRC16R.h"
#include "Crypto/Hash/CRC32.h"
#include "Crypto/Hash/CRC32R.h"
#include "Crypto/Hash/DJB2.h"
#include "Crypto/Hash/DJB2a.h"
#include "Crypto/Hash/ExcelHash.h"
#include "Crypto/Hash/FNV1.h"
#include "Crypto/Hash/FNV1a.h"
#include "Crypto/Hash/HashCreator.h"
#include "Crypto/Hash/MD5.h"
#include "Crypto/Hash/RIPEMD128.h"
#include "Crypto/Hash/RIPEMD160.h"
#include "Crypto/Hash/SDBM.h"
#include "Crypto/Hash/SHA1.h"
#include "Crypto/Hash/SHA1_SHA1.h"
#include "Crypto/Hash/SHA224.h"
#include "Crypto/Hash/SHA256.h"
#include "Crypto/Hash/SHA384.h"
#include "Crypto/Hash/SHA512.h"
//#include "Crypto/Hash/SuperFastHash.h"

Optional<Crypto::Hash::HashAlgorithm> Crypto::Hash::HashCreator::CreateHash(Crypto::Hash::HashType hashType)
{
	Crypto::Hash::HashAlgorithm *hash = 0;
	switch (hashType)
	{
	case Crypto::Hash::HashType::Adler32:
		NEW_CLASS(hash, Crypto::Hash::Adler32());
		break;
	case Crypto::Hash::HashType::CRC16:
		NEW_CLASS(hash, Crypto::Hash::CRC16());
		break;
	case Crypto::Hash::HashType::CRC16R:
		NEW_CLASS(hash, Crypto::Hash::CRC16R());
		break;
	case Crypto::Hash::HashType::CRC32:
		NEW_CLASS(hash, Crypto::Hash::CRC32());
		break;
	case Crypto::Hash::HashType::CRC32R_IEEE:
		NEW_CLASS(hash, Crypto::Hash::CRC32R(Crypto::Hash::CRC32::GetPolynormialIEEE()));
		break;
	case Crypto::Hash::HashType::CRC32C:
		NEW_CLASS(hash, Crypto::Hash::CRC32R(Crypto::Hash::CRC32::GetPolynormialCastagnoli()));
		break;
	case Crypto::Hash::HashType::DJB2:
		NEW_CLASS(hash, Crypto::Hash::DJB2());
		break;
	case Crypto::Hash::HashType::DJB2a:
		NEW_CLASS(hash, Crypto::Hash::DJB2a());
		break;
	case Crypto::Hash::HashType::Excel:
		NEW_CLASS(hash, Crypto::Hash::ExcelHash());
		break;
	case Crypto::Hash::HashType::FNV1:
		NEW_CLASS(hash, Crypto::Hash::FNV1());
		break;
	case Crypto::Hash::HashType::FNV1a:
		NEW_CLASS(hash, Crypto::Hash::FNV1a());
		break;
	case Crypto::Hash::HashType::MD5:
		NEW_CLASS(hash, Crypto::Hash::MD5());
		break;
	case Crypto::Hash::HashType::RIPEMD128:
		NEW_CLASS(hash, Crypto::Hash::RIPEMD128());
		break;
	case Crypto::Hash::HashType::RIPEMD160:
		NEW_CLASS(hash, Crypto::Hash::RIPEMD160());
		break;
	case Crypto::Hash::HashType::SDBM:
		NEW_CLASS(hash, Crypto::Hash::SDBM());
		break;
	case Crypto::Hash::HashType::SHA1:
		NEW_CLASS(hash, Crypto::Hash::SHA1());
		break;
	case Crypto::Hash::HashType::SHA224:
		NEW_CLASS(hash, Crypto::Hash::SHA224());
		break;
	case Crypto::Hash::HashType::SHA256:
		NEW_CLASS(hash, Crypto::Hash::SHA256());
		break;
	case Crypto::Hash::HashType::SHA384:
		NEW_CLASS(hash, Crypto::Hash::SHA384());
		break;
	case Crypto::Hash::HashType::SHA512:
		NEW_CLASS(hash, Crypto::Hash::SHA512());
		break;
//	case Crypto::Hash::HashType::SUPERFASTHASH:
//		NEW_CLASS(hash, Crypto::Hash::SuperFastHash());
//		break;
	case Crypto::Hash::HashType::SHA1_SHA1:
		NEW_CLASS(hash, Crypto::Hash::SHA1_SHA1());
		break;
	case Crypto::Hash::HashType::MD4:
	case Crypto::Hash::HashType::Unknown:
	default:
		hash = 0;
		break;
	}
	return hash;
}
