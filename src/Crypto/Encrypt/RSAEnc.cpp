#include "Stdafx.h"
#include "Crypto/Encrypt/RSAEnc.h"
#include <openssl/evp.h>

struct Crypto::Encrypt::RSAEnc::ClassData
{
	EVP_CIPHER_CTX *ctx;
};

Crypto::Encrypt::RSAEnc::RSAEnc()
{
	this->clsData = MemAllocNN(ClassData);
	this->clsData->ctx = EVP_CIPHER_CTX_new();
	EVP_CIPHER_CTX_init(this->clsData->ctx);
}

Crypto::Encrypt::RSAEnc::~RSAEnc()
{
	EVP_CIPHER_CTX_free(this->clsData->ctx);
	MemFreeNN(this->clsData);
}

Bool Crypto::Encrypt::RSAEnc::SetPublicKey(NN<Crypto::Cert::X509PubKey> key)
{
	if (this->clsData->ctx == 0)
		return false;
	
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
