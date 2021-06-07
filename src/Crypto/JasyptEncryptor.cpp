#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/JasyptEncryptor.h"
#include "Crypto/PBKDF2.h"
#include "Crypto/Encrypt/AES256.h"
#include "Crypto/Hash/HMAC.h"
#include "Crypto/Hash/SHA512.h"
#include "Text/TextBinEnc/Base64Enc.h"

const UInt8 *Crypto::JasyptEncryptor::DecGetSalt(const UInt8 *buff, UInt8 *salt)
{
	MemCopyNO(salt, buff, this->saltSize);
	return buff + this->saltSize;
}

const UInt8 *Crypto::JasyptEncryptor::DecGetIV(const UInt8 *buff, UInt8 *iv)
{
	MemCopyNO(iv, buff, this->ivSize);
	return buff + this->ivSize;
}

UOSInt Crypto::JasyptEncryptor::GetEncKey(const UInt8 *salt, UInt8 *key)
{
	Crypto::Hash::HMAC *hmac;
	Crypto::Hash::SHA512 *sha512;
	switch (this->keyAlgorithmn)
	{
	case KA_PBEWITHHMACSHA512:
		NEW_CLASS(sha512, Crypto::Hash::SHA512());
		NEW_CLASS(hmac, Crypto::Hash::HMAC(sha512, this->key, this->keyLen));
		Crypto::PBKDF2::Calc(salt, this->saltSize, this->iterCnt, this->dkLen, hmac, key);
		DEL_CLASS(hmac);
		DEL_CLASS(sha512);
		return this->dkLen;
	}
	return 0;
}

Crypto::Encrypt::ICrypto *Crypto::JasyptEncryptor::CreateCrypto(const UInt8 *iv, const UInt8 *keyBuff)
{
	Crypto::Encrypt::BlockCipher *bCipher;
	switch (this->cipherAlgorithm)
	{
	case CA_AES256:
	default:
		NEW_CLASS(bCipher, Crypto::Encrypt::AES256(keyBuff));
		bCipher->SetChainMode(Crypto::Encrypt::BlockCipher::CM_CBC);
		bCipher->SetIV(iv);
		return bCipher;
	}
}

Crypto::JasyptEncryptor::JasyptEncryptor(KeyAlgorithm keyAlg, CipherAlgorithm cipherAlg, const UInt8 *key, UOSInt keyLen)
{
	this->keyAlgorithmn = keyAlg;
	this->cipherAlgorithm = cipherAlg;
	switch (this->cipherAlgorithm)
	{
	case CA_AES256:
		this->saltSize = 16;
		this->ivSize = 16;
		this->iterCnt = 1000;
		this->dkLen = 32;
		break;
	default:
		this->saltSize = 16;
		this->ivSize = 16;
		this->iterCnt = 1000;
		this->dkLen = 32;
		break;
	}
	this->key = MemAlloc(UInt8, keyLen);
	MemCopyNO(this->key, key, keyLen);
	this->keyLen = keyLen;
}

Crypto::JasyptEncryptor::~JasyptEncryptor()
{
	MemFree(this->key);
}

UOSInt Crypto::JasyptEncryptor::Decrypt(const UInt8 *srcBuff, UOSInt srcLen, UInt8 *outBuff)
{
	if (srcLen < this->ivSize + this->saltSize)
	{
		return 0;
	}
	UOSInt outSize;
	const UInt8 *srcBuffEnd = srcBuff + srcLen;
	UInt8 *salt = MemAlloc(UInt8, this->saltSize);
	UInt8 *iv = MemAlloc(UInt8, this->ivSize);
	UInt8 *key = MemAlloc(UInt8, this->dkLen);
	srcBuff = this->DecGetSalt(srcBuff, salt);
	srcBuff = this->DecGetIV(srcBuff, iv);
	this->GetEncKey(salt, key);
	Crypto::Encrypt::ICrypto *enc = this->CreateCrypto(iv, key);
	outSize = enc->Decrypt(srcBuff, (UOSInt)(srcBuffEnd - srcBuff), outBuff, 0);
	DEL_CLASS(enc);
	MemFree(key);
	MemFree(iv);
	MemFree(salt);
	while (outSize > 0 && outBuff[outSize - 1] == 8)
	{
		outSize--;
	}
	return outSize;
}

UOSInt Crypto::JasyptEncryptor::DecryptB64(const UTF8Char *b64Buff, UOSInt b64Len, UInt8 *outBuff)
{
	Text::TextBinEnc::Base64Enc b64;
	UInt8 *tmpBuff = MemAlloc(UInt8, b64Len);
	UOSInt retSize;
	retSize = b64.DecodeBin(b64Buff, b64Len, tmpBuff);
	retSize = this->Decrypt(tmpBuff, retSize, outBuff);
	MemFree(tmpBuff);
	return retSize;
}

UOSInt Crypto::JasyptEncryptor::DecryptB64(const UTF8Char *b64Buff, UInt8 *outBuff)
{
	Text::TextBinEnc::Base64Enc b64;
	UInt8 *tmpBuff = MemAlloc(UInt8, Text::StrCharCnt(b64Buff));
	UOSInt retSize;
	retSize = b64.DecodeBin(b64Buff, tmpBuff);
	retSize = this->Decrypt(tmpBuff, retSize, outBuff);
	MemFree(tmpBuff);
	return retSize;
}
