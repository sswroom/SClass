#include "Stdafx.h"
#include "Crypto/Encrypt/AES256GCM.h"
#include <windows.h>
#include <bcrypt.h>

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
	BCRYPT_ALG_HANDLE       hAlg = NULL;
	BCRYPT_KEY_HANDLE       hKey = NULL;
	NTSTATUS                status;
	status = BCryptOpenAlgorithmProvider(
		&hAlg,
		BCRYPT_AES_ALGORITHM,
		MS_PRIMITIVE_PROVIDER,
		0);
	if (status != 0)
	{
		printf("Error in BCryptOpenAlgorithmProvider: status = 0x%lx\r\n", status);
		return 0;
	}

	status = BCryptSetProperty(
		hAlg,
		BCRYPT_CHAINING_MODE,
		(PBYTE)BCRYPT_CHAIN_MODE_GCM,
		sizeof(BCRYPT_CHAIN_MODE_GCM),
		0);
	if (status != 0)
	{
		printf("Error in BCryptSetProperty: status = 0x%lx\r\n", status);
		BCryptCloseAlgorithmProvider(hAlg, 0);
		return 0;
	}
	UInt8 keyBuff[sizeof(BCRYPT_KEY_DATA_BLOB_HEADER) + 32];
	BCRYPT_KEY_DATA_BLOB_HEADER *keyHdr = (BCRYPT_KEY_DATA_BLOB_HEADER*)keyBuff;
	keyHdr->dwMagic = BCRYPT_KEY_DATA_BLOB_MAGIC;
	keyHdr->dwVersion = BCRYPT_KEY_DATA_BLOB_VERSION1;
	keyHdr->cbKeyData = 32;
	MemCopyNO(&keyBuff[sizeof(BCRYPT_KEY_DATA_BLOB_HEADER)], this->key, 32);
	status = BCryptImportKey(
		hAlg,
		0,
		BCRYPT_KEY_DATA_BLOB,
		&hKey,
		0,
		0,
		keyBuff,
		sizeof(keyBuff),
		0);
	if (status != 0)
	{
		printf("Error in BCryptImportKey: status = 0x%lx\r\n", status);
		BCryptCloseAlgorithmProvider(hAlg, 0);
		return 0;
	}

	BCRYPT_AUTHENTICATED_CIPHER_MODE_INFO cipherInfo;
	cipherInfo.dwInfoVersion = 1;
	cipherInfo.cbSize = sizeof(cipherInfo);
	cipherInfo.pbNonce = this->iv;
	cipherInfo.cbNonce = 12;
	cipherInfo.pbAuthData = 0;
	cipherInfo.cbAuthData = 0;
	cipherInfo.pbTag = (PUCHAR)&outBuff[inSize];
	cipherInfo.cbTag = 16;
	cipherInfo.pbMacContext = 0;
	cipherInfo.cbMacContext = 0;
	cipherInfo.cbAAD = 0;
	cipherInfo.cbData = 0;
	cipherInfo.dwFlags = 0;

	ULONG cbData = 0;
	status = BCryptEncrypt(hKey, (PUCHAR)inBuff.Ptr(), (ULONG)inSize,
		(PBYTE)&cipherInfo,
		NULL,
		0,
		outBuff.Ptr(),
		inSize,
		&cbData,
		0);
	if (status != 0)
	{
		printf("Error in BCryptDecrypt: status = 0x%lx\r\n", status);
	}
	BCryptDestroyKey(hKey);
	BCryptCloseAlgorithmProvider(hAlg, 0);
	return cbData + 16;
}

UOSInt Crypto::Encrypt::AES256GCM::Decrypt(UnsafeArray<const UInt8> inBuff, UOSInt inSize, UnsafeArray<UInt8> outBuff)
{
	BCRYPT_ALG_HANDLE       hAlg = NULL;
	BCRYPT_KEY_HANDLE       hKey = NULL;
	NTSTATUS                status;
	status = BCryptOpenAlgorithmProvider(
		&hAlg,
		BCRYPT_AES_ALGORITHM,
		MS_PRIMITIVE_PROVIDER,
		0);
	if (status != 0)
	{
		printf("Error in BCryptOpenAlgorithmProvider: status = 0x%lx\r\n", status);
		return 0;
	}

	status = BCryptSetProperty(
		hAlg,
		BCRYPT_CHAINING_MODE,
		(PBYTE)BCRYPT_CHAIN_MODE_GCM,
		sizeof(BCRYPT_CHAIN_MODE_GCM),
		0);
	if (status != 0)
	{
		printf("Error in BCryptSetProperty: status = 0x%lx\r\n", status);
		BCryptCloseAlgorithmProvider(hAlg, 0);
		return 0;
	}
	UInt8 keyBuff[sizeof(BCRYPT_KEY_DATA_BLOB_HEADER) + 32];
	BCRYPT_KEY_DATA_BLOB_HEADER *keyHdr = (BCRYPT_KEY_DATA_BLOB_HEADER*)keyBuff;
	keyHdr->dwMagic = BCRYPT_KEY_DATA_BLOB_MAGIC;
	keyHdr->dwVersion = BCRYPT_KEY_DATA_BLOB_VERSION1;
	keyHdr->cbKeyData = 32;
	MemCopyNO(&keyBuff[sizeof(BCRYPT_KEY_DATA_BLOB_HEADER)], this->key, 32);
	status = BCryptImportKey(
		hAlg,
		0,
		BCRYPT_KEY_DATA_BLOB,
		&hKey,
		0,
		0,
		keyBuff,
		sizeof(keyBuff),
		0);
	if (status != 0)
	{
		printf("Error in BCryptImportKey: status = 0x%lx\r\n", status);
		BCryptCloseAlgorithmProvider(hAlg, 0);
		return 0;
	}

	BCRYPT_AUTHENTICATED_CIPHER_MODE_INFO cipherInfo;
	cipherInfo.dwInfoVersion = 1;
	cipherInfo.cbSize = sizeof(cipherInfo);
	cipherInfo.pbNonce = this->iv;
	cipherInfo.cbNonce = 12;
	cipherInfo.pbAuthData = 0;
	cipherInfo.cbAuthData = 0;
	cipherInfo.pbTag = (PUCHAR)&inBuff[inSize - 16];
	cipherInfo.cbTag = 16;
	cipherInfo.pbMacContext = 0;
	cipherInfo.cbMacContext = 0;
	cipherInfo.cbAAD = 0;
	cipherInfo.cbData = 0;
	cipherInfo.dwFlags = 0;

	ULONG cbData = 0;
	status = BCryptDecrypt(hKey, (PUCHAR)inBuff.Ptr(), (ULONG)inSize - 16,
		(PBYTE)&cipherInfo,
		NULL,
		0,
		outBuff.Ptr(),
		inSize,
		&cbData,
		0);
	if (status != 0)
	{
		printf("Error in BCryptDecrypt: status = 0x%lx\r\n", status);
	}
	BCryptDestroyKey(hKey);
	BCryptCloseAlgorithmProvider(hAlg, 0);
	return cbData;
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
