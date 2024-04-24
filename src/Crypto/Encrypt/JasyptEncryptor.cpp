#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/PBKDF2.h"
#include "Crypto/Encrypt/AES256.h"
#include "Crypto/Encrypt/JasyptEncryptor.h"
#include "Crypto/Hash/HMAC.h"
#include "Crypto/Hash/SHA512.h"
#include "Data/ByteTool.h"
#include "Text/TextBinEnc/Base64Enc.h"

const UInt8 *Crypto::Encrypt::JasyptEncryptor::DecGetSalt(const UInt8 *buff, UInt8 *salt)
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

const UInt8 *Crypto::Encrypt::JasyptEncryptor::DecGetIV(const UInt8 *buff, UInt8 *iv)
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

UOSInt Crypto::Encrypt::JasyptEncryptor::GetEncKey(const UInt8 *salt, UInt8 *key)
{
	switch (this->keyAlgorithmn)
	{
	case KA_PBEWITHHMACSHA512:
		{
			Crypto::Hash::SHA512 sha512;
			Crypto::Hash::HMAC hmac(sha512, this->key.Ptr(), this->key.GetSize());
			Crypto::PBKDF2::Calc(salt, this->saltSize, this->iterCnt, this->dkLen, hmac, key);
		}
		return this->dkLen;
	}
	return 0;
}

NotNullPtr<Crypto::Encrypt::ICrypto> Crypto::Encrypt::JasyptEncryptor::CreateCrypto(const UInt8 *iv, const UInt8 *keyBuff)
{
	NotNullPtr<Crypto::Encrypt::BlockCipher> bCipher;
	switch (this->cipherAlgorithm)
	{
	case CA_AES256:
	default:
		NEW_CLASSNN(bCipher, Crypto::Encrypt::AES256(keyBuff));
		bCipher->SetChainMode(Crypto::Encrypt::ChainMode::CBC);
		bCipher->SetIV(iv);
		return bCipher;
	}
}

Crypto::Encrypt::JasyptEncryptor::JasyptEncryptor(KeyAlgorithm keyAlg, CipherAlgorithm cipherAlg, Data::ByteArrayR key) : key(key)
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
}

Crypto::Encrypt::JasyptEncryptor::~JasyptEncryptor()
{
	if (this->salt)
	{
		MemFree(this->salt);
	}
	if (this->iv)
	{
		MemFree(this->iv);
	}
}

Bool Crypto::Encrypt::JasyptEncryptor::Decrypt(NotNullPtr<IO::ConfigFile> cfg)
{
	Data::ArrayListStringNN cateList;
	Data::ArrayListStringNN keyList;
	NotNullPtr<Text::String> cate;
	NotNullPtr<Text::String> key;
	NotNullPtr<Text::String> val;
	UInt8 buff[256];
	UOSInt buffSize;
	cfg->GetCateList(cateList, true);
	Data::ArrayIterator<NotNullPtr<Text::String>> it = cateList.Iterator();
	while (it.HasNext())
	{
		cate = it.Next();
		keyList.Clear();
		cfg->GetKeys(cate, keyList);
		Data::ArrayIterator<NotNullPtr<Text::String>> it = keyList.Iterator();
		while (it.HasNext())
		{
			key = it.Next();
			if (cfg->GetCateValue(cate, key).SetTo(val) && val->StartsWith(UTF8STRC("ENC(")) && val->EndsWith(')'))
			{
				buffSize = this->DecryptB64(Text::CStringNN(&val->v[4], val->leng - 5), buff);
				if (buffSize > 0)
				{
					val = Text::String::New(buff, buffSize);
					cfg->SetValue(cate, key, val.Ptr());
					val->Release();
				}
			}
		}
	}
	return true;
}

UOSInt Crypto::Encrypt::JasyptEncryptor::Decrypt(const UInt8 *srcBuff, UOSInt srcLen, UInt8 *outBuff)
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
	NotNullPtr<Crypto::Encrypt::ICrypto> enc = this->CreateCrypto(iv, key);
	outSize = enc->Decrypt(srcBuff, (UOSInt)(srcBuffEnd - srcBuff), outBuff);
	enc.Delete();
	MemFree(key);
	MemFree(iv);
	MemFree(salt);
	while (outSize > 0 && outBuff[outSize - 1] < 32)
	{
		outSize--;
	}
	return outSize;
}

UOSInt Crypto::Encrypt::JasyptEncryptor::DecryptB64(Text::CStringNN b64Str, UInt8 *outBuff)
{
	Text::TextBinEnc::Base64Enc b64;
	UInt8 *tmpBuff = MemAlloc(UInt8, b64Str.leng);
	UOSInt retSize;
	retSize = b64.DecodeBin(b64Str.v, b64Str.leng, tmpBuff);
	retSize = this->Decrypt(tmpBuff, retSize, outBuff);
	MemFree(tmpBuff);
	return retSize;
}

UOSInt Crypto::Encrypt::JasyptEncryptor::EncryptAsB64(NotNullPtr<Text::StringBuilderUTF8> sb, Data::ByteArrayR srcBuff)
{
	UInt8 *srcTmpBuff = 0;
	UOSInt nBlock = srcBuff.GetSize() / this->ivSize;
	UOSInt destLen;
	if (nBlock * this->ivSize != srcBuff.GetSize())
	{
		destLen = (nBlock + 1) * this->ivSize;
		srcTmpBuff = MemAlloc(UInt8, destLen);
		MemCopyNO(srcTmpBuff, srcBuff.Ptr(), srcBuff.GetSize());
		MemFillB(&srcTmpBuff[srcBuff.GetSize()], destLen - srcBuff.GetSize(), (UInt8)(destLen - srcBuff.GetSize()));
		srcBuff = Data::ByteArrayR(srcTmpBuff, destLen);
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
		this->random.NextBytes(&destBuff[destOfst], this->saltSize);
		destOfst += this->saltSize;
	}
	if (this->iv != 0)
	{
		iv = this->iv;
	}
	else
	{
		iv = &destBuff[destOfst];
		this->random.NextBytes(&destBuff[destOfst], this->ivSize);
		destOfst += this->ivSize;
	}
	UInt8 *key = MemAlloc(UInt8, this->dkLen);
	this->GetEncKey(salt, key);
	NotNullPtr<Crypto::Encrypt::ICrypto> enc = this->CreateCrypto(iv, key);
	destOfst += enc->Encrypt(srcBuff.Ptr(), destLen - destOfst, &destBuff[destOfst]);
	enc.Delete();
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

Text::CStringNN Crypto::Encrypt::JasyptEncryptor::KeyAlgorithmGetName(KeyAlgorithm keyAlg)
{
	switch (keyAlg)
	{
	case KA_PBEWITHHMACSHA512:
		return CSTR("PBWITHMACSHA512");
	default:
		return CSTR("Unknown");
	}
}

Text::CStringNN Crypto::Encrypt::JasyptEncryptor::CipherAlgorithmGetName(CipherAlgorithm cipherAlg)
{
	switch (cipherAlg)
	{
	case CA_AES256:
		return CSTR("AES-256");
	default:
		return CSTR("Unknown");
	}
}
