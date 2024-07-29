#include "Stdafx.h"
#include "Crypto/Encrypt/AES256GCM.h"
#include <openssl/evp.h>

//#define VERBOSE

Crypto::Encrypt::AES256GCM::AES256GCM(UnsafeArray<const UInt8> key, UnsafeArray<const UInt8> iv)
{
	MemCopyNO(this->key, key.Ptr(), 32);
	MemCopyNO(this->iv, iv.Ptr(), 12);
}

Crypto::Encrypt::AES256GCM::~AES256GCM()
{

}

UOSInt Crypto::Encrypt::AES256GCM::Encrypt(UnsafeArray<const UInt8> inBuff, UOSInt inSize, UnsafeArray<UInt8> outBuff)
{
	int ret;
	EVP_CIPHER_CTX *ectx = EVP_CIPHER_CTX_new();
	ret = EVP_EncryptInit(ectx, EVP_aes_256_gcm(), this->key, this->iv);
#if defined(VERBOSE)
	printf("EVP_EncryptInit: ret = %d\r\n", ret);
#endif
	if (ret != 1)
		return 0;
	int outSize = 0;
	ret = EVP_EncryptUpdate(ectx, outBuff.Ptr(), &outSize, inBuff.Ptr(), (int)inSize);
#if defined(VERBOSE)
	printf("EVP_EncryptUpdate: ret = %d, outSize = %d\r\n", ret, outSize);
#endif
	int finalSize = 0;
	ret = EVP_EncryptFinal(ectx, &outBuff[outSize], &finalSize);
#if defined(VERBOSE)
	printf("EVP_EncryptFinal: ret = %d, finalSize = %d\r\n", ret, finalSize);
#endif
	ret = EVP_CIPHER_CTX_ctrl(ectx, EVP_CTRL_GCM_GET_TAG, 16, &outBuff[outSize + finalSize]);
#if defined(VERBOSE)
	printf("EVP_CIPHER_CTX_ctrl: ret = %d\r\n", ret);
#endif
	EVP_CIPHER_CTX_free(ectx);
	return (UOSInt)(outSize + finalSize + 16);
}

UOSInt Crypto::Encrypt::AES256GCM::Decrypt(UnsafeArray<const UInt8> inBuff, UOSInt inSize, UnsafeArray<UInt8> outBuff)
{
	int ret;
	EVP_CIPHER_CTX *ectx = EVP_CIPHER_CTX_new();
	ret = EVP_DecryptInit(ectx, EVP_aes_256_gcm(), this->key, this->iv);
#if defined(VERBOSE)
	printf("EVP_DecryptInit: ret = %d\r\n", ret);
#endif
	if (ret != 1)
		return 0;
	int outSize = 0;
	ret = EVP_DecryptUpdate(ectx, outBuff.Ptr(), &outSize, inBuff.Ptr(), (int)inSize - 16);
#if defined(VERBOSE)
	printf("EVP_DecryptUpdate: ret = %d, outSize = %d\r\n", ret, outSize);
#endif
	int finalSize = 0;
	ret = EVP_CIPHER_CTX_ctrl(ectx, EVP_CTRL_GCM_SET_TAG, 16, (void*)&inBuff[inSize - 16]);
#if defined(VERBOSE)
	printf("EVP_CIPHER_CTX_ctrl: ret = %d\r\n", ret);
#endif
	ret = EVP_DecryptFinal(ectx, &outBuff[outSize], &finalSize);
#if defined(VERBOSE)
	printf("EVP_DecryptFinal: ret = %d, finalSize = %d\r\n", ret, finalSize);
#endif
	EVP_CIPHER_CTX_free(ectx);
	return (UOSInt)(outSize + finalSize);
}

UOSInt Crypto::Encrypt::AES256GCM::GetEncBlockSize() const
{
	return 16;
}

UOSInt Crypto::Encrypt::AES256GCM::GetDecBlockSize() const
{
	return 16;
}

void Crypto::Encrypt::AES256GCM::SetIV(UnsafeArray<const UInt8> iv)
{
	MemCopyNO(this->iv, iv.Ptr(), 12);
}
