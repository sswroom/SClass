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

Crypto::Hash::IHash *Crypto::Hash::HashCreator::CreateHash(Crypto::Hash::HashType hashType)
{
	Crypto::Hash::IHash *hash = 0;
	switch (hashType)
	{
	case Crypto::Hash::HT_ADLER32:
		NEW_CLASS(hash, Crypto::Hash::Adler32());
		break;
	case Crypto::Hash::HT_CRC16:
		NEW_CLASS(hash, Crypto::Hash::CRC16());
		break;
	case Crypto::Hash::HT_CRC16R:
		NEW_CLASS(hash, Crypto::Hash::CRC16R());
		break;
	case Crypto::Hash::HT_CRC32:
		NEW_CLASS(hash, Crypto::Hash::CRC32());
		break;
	case Crypto::Hash::HT_CRC32R_IEEE:
		NEW_CLASS(hash, Crypto::Hash::CRC32R(Crypto::Hash::CRC32R::GetPolynormialIEEE()));
		break;
	case Crypto::Hash::HT_DJB2:
		NEW_CLASS(hash, Crypto::Hash::DJB2());
		break;
	case Crypto::Hash::HT_DJB2A:
		NEW_CLASS(hash, Crypto::Hash::DJB2a());
		break;
	case Crypto::Hash::HT_EXCEL:
		NEW_CLASS(hash, Crypto::Hash::ExcelHash());
		break;
	case Crypto::Hash::HT_FNV1:
		NEW_CLASS(hash, Crypto::Hash::FNV1());
		break;
	case Crypto::Hash::HT_FNV1A:
		NEW_CLASS(hash, Crypto::Hash::FNV1a());
		break;
	case Crypto::Hash::HT_MD5:
		NEW_CLASS(hash, Crypto::Hash::MD5());
		break;
	case Crypto::Hash::HT_RIPEMD128:
		NEW_CLASS(hash, Crypto::Hash::RIPEMD128());
		break;
	case Crypto::Hash::HT_RIPEMD160:
		NEW_CLASS(hash, Crypto::Hash::RIPEMD160());
		break;
	case Crypto::Hash::HT_SDBM:
		NEW_CLASS(hash, Crypto::Hash::SDBM());
		break;
	case Crypto::Hash::HT_SHA1:
		NEW_CLASS(hash, Crypto::Hash::SHA1());
		break;
	case Crypto::Hash::HT_SHA224:
		NEW_CLASS(hash, Crypto::Hash::SHA224());
		break;
	case Crypto::Hash::HT_SHA256:
		NEW_CLASS(hash, Crypto::Hash::SHA256());
		break;
	case Crypto::Hash::HT_SHA384:
		NEW_CLASS(hash, Crypto::Hash::SHA384());
		break;
	case Crypto::Hash::HT_SHA512:
		NEW_CLASS(hash, Crypto::Hash::SHA512());
		break;
//	case Crypto::Hash::HT_SUPERFASTHASH:
//		NEW_CLASS(hash, Crypto::Hash::SuperFastHash());
//		break;
	case Crypto::Hash::HT_SHA1_SHA1:
		NEW_CLASS(hash, Crypto::Hash::SHA1_SHA1());
		break;
	case Crypto::Hash::HT_UNKNOWN:
	default:
		hash = 0;
		break;
	}
	return hash;
}
