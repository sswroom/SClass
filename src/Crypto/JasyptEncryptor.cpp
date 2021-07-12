#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/JasyptEncryptor.h"
#include "Crypto/PBKDF2.h"
#include "Crypto/Encrypt/AES256.h"
#include "Crypto/Hash/HMAC.h"
#include "Crypto/Hash/SHA512.h"
#include "Data/ByteTool.h"
#include "Text/TextBinEnc/Base64Enc.h"

const UInt8 *Crypto::JasyptEncryptor::DecGetSalt(const UInt8 *buff, UInt8 *salt)
{
	if (this->salt)
	{
		MemCopyNO(salt, this->salt, this->saltSize);
		return buff;
	}
	else
	{
		MemCopyNO(salt, buff, this->saltSize);
		return buff + this->saltSize;
	}
}

const UInt8 *Crypto::JasyptEncryptor::DecGetIV(const UInt8 *buff, UInt8 *iv)
{
	if (this->iv)
	{
		MemCopyNO(iv, this->iv, this->ivSize);
		return buff;
	}
	else
	{
		MemCopyNO(iv, buff, this->ivSize);
		return buff + this->ivSize;
	}
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
	this->salt = 0;
	this->iv = 0;
	NEW_CLASS(this->random, Data::RandomBytesGenerator());
	this->key = MemAlloc(UInt8, keyLen);
	MemCopyNO(this->key, key, keyLen);
	this->keyLen = keyLen;
}

Crypto::JasyptEncryptor::~JasyptEncryptor()
{
	MemFree(this->key);
	if (this->salt)
	{
		MemFree(this->salt);
	}
	if (this->iv)
	{
		MemFree(this->iv);
	}
	SDEL_CLASS(this->random);
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

UOSInt Crypto::JasyptEncryptor::EncryptAsB64(Text::StringBuilderUTF *sb, const UInt8 *srcBuff, UOSInt srcLen)
{
	UInt8 *srcTmpBuff = 0;
	UOSInt nBlock = srcLen / this->ivSize;
	UOSInt destLen;
	if (nBlock * this->ivSize != srcLen)
	{
		destLen = (nBlock + 1) * this->ivSize;
		srcTmpBuff = MemAlloc(UInt8, destLen);
		MemCopyNO(srcTmpBuff, srcBuff, srcLen);
		MemFillB(&srcTmpBuff[srcLen], destLen - srcLen, 8);
		srcBuff = srcTmpBuff;
	}
	else
	{
		destLen = nBlock * this->ivSize;		
	}
	if (this->salt == 0)
	{
		destLen += this->saltSize;
	}
	if (this->iv == 0)
	{
		destLen += this->ivSize;
	}
	UOSInt destOfst = 0;
	UInt8 *destBuff = MemAlloc(UInt8, destLen);
	const UInt8 *salt;
	const UInt8 *iv;
	if (this->salt != 0)
	{
		salt = this->salt;
	}
	else
	{
		salt = &destBuff[destOfst];
		this->random->NextBytes(&destBuff[destOfst], this->saltSize);
		destOfst += this->saltSize;
	}
	if (this->iv != 0)
	{
		iv = this->iv;
	}
	else
	{
		iv = &destBuff[destOfst];
		this->random->NextBytes(&destBuff[destOfst], this->ivSize);
		destOfst += this->ivSize;
	}
	UInt8 *key = MemAlloc(UInt8, this->dkLen);
	this->GetEncKey(salt, key);
	Crypto::Encrypt::ICrypto *enc = this->CreateCrypto(iv, key);
	destOfst += enc->Encrypt(srcBuff, destLen - destOfst, &destBuff[destOfst], 0);
	DEL_CLASS(enc);
	MemFree(key);
	if (srcTmpBuff)
	{
		MemFree(srcTmpBuff);
	}

	Text::TextBinEnc::Base64Enc b64;
	b64.EncodeBin(sb, destBuff, destOfst);
	MemFree(destBuff);
	return sb->GetCharCnt();
}

const UTF8Char *Crypto::JasyptEncryptor::GetKeyAlgorithmName(KeyAlgorithm keyAlg)
{
	switch (keyAlg)
	{
	case KA_PBEWITHHMACSHA512:
		return (const UTF8Char*)"PBWITHMACSHA512";
	default:
		return (const UTF8Char*)"Unknown";
	}
}

const UTF8Char *Crypto::JasyptEncryptor::GetCipherAlgorithmName(CipherAlgorithm cipherAlg)
{
	switch (cipherAlg)
	{
	case CA_AES256:
		return (const UTF8Char*)"AES-256";
	default:
		return (const UTF8Char*)"Unknown";
	}
}
