#include "Stdafx.h"
#include "Crypto/Encrypt/AES128GCM.h"
#include <openssl/evp.h>

//#define VERBOSE

Crypto::Encrypt::AES128GCM::AES128GCM(UnsafeArray<const UInt8> key) : BlockCipher(16, 12)
{
	MemCopyNO(this->key, key.Ptr(), 16);
}

Crypto::Encrypt::AES128GCM::~AES128GCM()
{

}

UIntOS Crypto::Encrypt::AES128GCM::Encrypt(UnsafeArray<const UInt8> inBuff, UIntOS inSize, UnsafeArray<UInt8> outBuff)
{
	int ret;
	EVP_CIPHER_CTX *ectx = EVP_CIPHER_CTX_new();
	ret = EVP_EncryptInit(ectx, EVP_aes_128_gcm(), this->key, this->iv.Ptr());
#if defined(VERBOSE)
	printf("EVP_EncryptInit: ret = %d\r\n", ret);
#endif
	if (ret != 1)
		return 0;
	ret = EVP_CIPHER_CTX_set_padding(ectx, this->pad == PaddingMode::None ? 0 : 1);
#if defined(VERBOSE)
	printf("EVP_CIPHER_CTX_set_padding: ret = %d\r\n", ret);
#endif
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
	return (UIntOS)(outSize + finalSize + 16);
}

UIntOS Crypto::Encrypt::AES128GCM::Decrypt(UnsafeArray<const UInt8> inBuff, UIntOS inSize, UnsafeArray<UInt8> outBuff)
{
	int ret;
	EVP_CIPHER_CTX *ectx = EVP_CIPHER_CTX_new();
	ret = EVP_DecryptInit(ectx, EVP_aes_128_gcm(), this->key, this->iv.Ptr());
#if defined(VERBOSE)
	printf("EVP_DecryptInit: ret = %d\r\n", ret);
#endif
	if (ret != 1)
		return 0;
	ret = EVP_CIPHER_CTX_set_padding(ectx, this->pad == PaddingMode::None ? 0 : 1);
#if defined(VERBOSE)
	printf("EVP_CIPHER_CTX_set_padding: ret = %d\r\n", ret);
#endif
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
	return (UIntOS)(outSize + finalSize);
}

UIntOS Crypto::Encrypt::AES128GCM::EncryptBlock(UnsafeArray<const UInt8> inBlock, UnsafeArray<UInt8> outBlock) const
{
	return 16;
}

UIntOS Crypto::Encrypt::AES128GCM::DecryptBlock(UnsafeArray<const UInt8> inBlock, UnsafeArray<UInt8> outBlock) const
{
	return 16;
}
