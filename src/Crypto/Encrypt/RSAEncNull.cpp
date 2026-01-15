#include "Stdafx.h"
#include "Crypto/Encrypt/RSAEnc.h"

struct Crypto::Encrypt::RSAEnc::ClassData
{
};

Crypto::Encrypt::RSAEnc::RSAEnc()
{
	this->clsData = MemAllocNN(ClassData);
}

Crypto::Encrypt::RSAEnc::~RSAEnc()
{
	MemFreeNN(this->clsData);
}

Bool Crypto::Encrypt::RSAEnc::SetPublicKey(NN<Crypto::Cert::X509PubKey> key)
{
	return false;
}

Bool Crypto::Encrypt::RSAEnc::LoadPublicKeyFile(Text::CString filePath)
{
	return false;
}

UnsafeArrayOpt<const UInt8> Crypto::Encrypt::RSAEnc::GetSessKey()
{
	return nullptr;
}

UIntOS Crypto::Encrypt::RSAEnc::GetSessKeyLen()
{
	return 0;
}

UnsafeArrayOpt<const UInt8> Crypto::Encrypt::RSAEnc::GetIV()
{
	return nullptr;
}

UIntOS Crypto::Encrypt::RSAEnc::GetIVLen()
{
	return 0;
}
