#include "Stdafx.h"
#include "Crypto/Encrypt/RSAEnc.h"

struct Crypto::Encrypt::RSAEnc::ClassData
{
};

Crypto::Encrypt::RSAEnc::RSAEnc()
{
	this->clsData = MemAlloc(ClassData, 1);
}

Crypto::Encrypt::RSAEnc::~RSAEnc()
{
	MemFree(this->clsData);
}

Bool Crypto::Encrypt::RSAEnc::SetPublicKey(Crypto::Cert::X509PubKey *key)
{
	return false;
}

Bool Crypto::Encrypt::RSAEnc::LoadPublicKeyFile(Text::CString filePath)
{
	return false;
}

UnsafeArrayOpt<const UInt8> Crypto::Encrypt::RSAEnc::GetSessKey()
{
	return 0;
}

UOSInt Crypto::Encrypt::RSAEnc::GetSessKeyLen()
{
	return 0;
}

UnsafeArrayOpt<const UInt8> Crypto::Encrypt::RSAEnc::GetIV()
{
	return 0;
}

UOSInt Crypto::Encrypt::RSAEnc::GetIVLen()
{
	return 0;
}
