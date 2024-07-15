#include "Stdafx.h"
#include "Crypto/Encrypt/AES128GCM.h"
#include <openssl/evp.h>

Crypto::Encrypt::AES128GCM::AES128GCM(UnsafeArray<const UInt8> key, UnsafeArray<const UInt8> iv)
{
	MemCopyNO(this->key, key.Ptr(), 16);
	MemCopyNO(this->iv, iv.Ptr(), 12);
}

Crypto::Encrypt::AES128GCM::~AES128GCM()
{

}

UOSInt Crypto::Encrypt::AES128GCM::Encrypt(UnsafeArray<const UInt8> inBuff, UOSInt inSize, UnsafeArray<UInt8> outBuff)
{
	int ret;
	EVP_CIPHER_CTX *ectx = EVP_CIPHER_CTX_new();
	ret = EVP_EncryptInit(ectx, EVP_aes_128_gcm(), this->key, this->iv);
	printf("EVP_EncryptInit: ret = %d\r\n", ret);
	int outSize = 0;
	ret = EVP_EncryptUpdate(ectx, outBuff.Ptr(), &outSize, inBuff.Ptr(), (int)inSize);
	printf("EVP_EncryptUpdate: ret = %d, outSize = %d\r\n", ret, outSize);
	int finalSize = 0;
	ret = EVP_EncryptFinal(ectx, &outBuff[outSize], &finalSize);
	printf("EVP_EncryptFinal: ret = %d, finalSize = %d\r\n", ret, finalSize);
	ret = EVP_CIPHER_CTX_ctrl(ectx, EVP_CTRL_GCM_GET_TAG, 16, &outBuff[outSize + finalSize]);
	printf("EVP_CIPHER_CTX_ctrl: ret = %d\r\n", ret);
	EVP_CIPHER_CTX_free(ectx);
	return (UOSInt)(outSize + finalSize + 16);
}

UOSInt Crypto::Encrypt::AES128GCM::Decrypt(UnsafeArray<const UInt8> inBuff, UOSInt inSize, UnsafeArray<UInt8> outBuff)
{
	int ret;
	EVP_CIPHER_CTX *ectx = EVP_CIPHER_CTX_new();
	ret = EVP_DecryptInit(ectx, EVP_aes_128_gcm(), this->key, this->iv);
	printf("EVP_DecryptInit: ret = %d\r\n", ret);
	int outSize = 0;
	ret = EVP_DecryptUpdate(ectx, outBuff.Ptr(), &outSize, inBuff.Ptr(), (int)inSize - 16);
	printf("EVP_DecryptUpdate: ret = %d, outSize = %d\r\n", ret, outSize);
	int finalSize = 0;
	ret = EVP_CIPHER_CTX_ctrl(ectx, EVP_CTRL_GCM_SET_TAG, 16, (void*)&inBuff[inSize - 16]);
	printf("EVP_CIPHER_CTX_ctrl: ret = %d\r\n", ret);
	ret = EVP_DecryptFinal(ectx, &outBuff[outSize], &finalSize);
	printf("EVP_DecryptFinal: ret = %d, finalSize = %d\r\n", ret, finalSize);
	EVP_CIPHER_CTX_free(ectx);
	return (UOSInt)(outSize + finalSize);
}

UOSInt Crypto::Encrypt::AES128GCM::GetEncBlockSize() const
{
	return 16;
}

UOSInt Crypto::Encrypt::AES128GCM::GetDecBlockSize() const
{
	return 16;
}
