#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/PBKDF2.h"
#include "Crypto/Encrypt/AES256.h"
#include "Crypto/Encrypt/JasyptEncryptor.h"
#include "Crypto/Hash/HMAC.h"
#include "Crypto/Hash/SHA512.h"
#include "Core/ByteTool_C.h"
#include "Text/TextBinEnc/Base64Enc.h"

UnsafeArray<const UInt8> Crypto::Encrypt::JasyptEncryptor::DecGetSalt(UnsafeArray<const UInt8> buff, UnsafeArray<UInt8> salt)
{
	UnsafeArray<UInt8> nnsalt;
	if (this->salt.SetTo(nnsalt))
	{
		MemCopyNO(salt.Ptr(), nnsalt.Ptr(), this->saltSize);
		return buff;
	}
	else
	{
		MemCopyNO(salt.Ptr(), buff.Ptr(), this->saltSize);
		return buff + this->saltSize;
	}
}

UnsafeArray<const UInt8> Crypto::Encrypt::JasyptEncryptor::DecGetIV(UnsafeArray<const UInt8> buff, UnsafeArray<UInt8> iv)
{
	UnsafeArray<UInt8> nniv;
	if (this->iv.SetTo(nniv))
	{
		MemCopyNO(iv.Ptr(), nniv.Ptr(), this->ivSize);
		return buff;
	}
	else
	{
		MemCopyNO(iv.Ptr(), buff.Ptr(), this->ivSize);
		return buff + this->ivSize;
	}
}

UOSInt Crypto::Encrypt::JasyptEncryptor::GetEncKey(UnsafeArray<const UInt8> salt, UnsafeArray<UInt8> key)
{
	switch (this->keyAlgorithmn)
	{
	case KA_PBEWITHHMACSHA512:
		{
			Crypto::Hash::SHA512 sha512;
			Crypto::Hash::HMAC hmac(sha512, this->key.Arr().Ptr(), this->key.GetSize());
			Crypto::PBKDF2::Calc(salt, this->saltSize, this->iterCnt, this->dkLen, hmac, key);
		}
		return this->dkLen;
	}
	return 0;
}

NN<Crypto::Encrypt::Encryption> Crypto::Encrypt::JasyptEncryptor::CreateCrypto(UnsafeArray<const UInt8> iv, UnsafeArray<const UInt8> keyBuff)
{
	NN<Crypto::Encrypt::BlockCipher> bCipher;
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
	this->salt = nullptr;
	this->iv = nullptr;
}

Crypto::Encrypt::JasyptEncryptor::~JasyptEncryptor()
{
	UnsafeArray<UInt8> p;
	if (this->salt.SetTo(p))
	{
		MemFreeArr(p);
	}
	if (this->iv.SetTo(p))
	{
		MemFreeArr(p);
	}
}

Bool Crypto::Encrypt::JasyptEncryptor::Decrypt(NN<IO::ConfigFile> cfg)
{
	Data::ArrayListStringNN cateList;
	Data::ArrayListStringNN keyList;
	NN<Text::String> cate;
	NN<Text::String> key;
	NN<Text::String> val;
	UInt8 buff[256];
	UOSInt buffSize;
	cfg->GetCateList(cateList, true);
	Data::ArrayIterator<NN<Text::String>> it = cateList.Iterator();
	while (it.HasNext())
	{
		cate = it.Next();
		keyList.Clear();
		cfg->GetKeys(cate, keyList);
		Data::ArrayIterator<NN<Text::String>> it = keyList.Iterator();
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

UOSInt Crypto::Encrypt::JasyptEncryptor::Decrypt(UnsafeArray<const UInt8> srcBuff, UOSInt srcLen, UnsafeArray<UInt8> outBuff)
{
	if (srcLen < this->ivSize + this->saltSize)
	{
		return 0;
	}
	UOSInt outSize;
	UnsafeArray<const UInt8> srcBuffEnd = srcBuff + srcLen;
	UnsafeArray<UInt8> salt = MemAllocArr(UInt8, this->saltSize);
	UnsafeArray<UInt8> iv = MemAllocArr(UInt8, this->ivSize);
	UnsafeArray<UInt8> key = MemAllocArr(UInt8, this->dkLen);
	srcBuff = this->DecGetSalt(srcBuff, salt);
	srcBuff = this->DecGetIV(srcBuff, iv);
	this->GetEncKey(salt, key);
	NN<Crypto::Encrypt::Encryption> enc = this->CreateCrypto(iv, key);
	outSize = enc->Decrypt(srcBuff, (UOSInt)(srcBuffEnd - srcBuff), outBuff);
	enc.Delete();
	MemFreeArr(key);
	MemFreeArr(iv);
	MemFreeArr(salt);
	while (outSize > 0 && outBuff[outSize - 1] < 32)
	{
		outSize--;
	}
	return outSize;
}

UOSInt Crypto::Encrypt::JasyptEncryptor::DecryptB64(Text::CStringNN b64Str, UnsafeArray<UInt8> outBuff)
{
	Text::TextBinEnc::Base64Enc b64;
	UnsafeArray<UInt8> tmpBuff = MemAllocArr(UInt8, b64Str.leng);
	UOSInt retSize;
	retSize = b64.DecodeBin(b64Str, tmpBuff);
	retSize = this->Decrypt(tmpBuff, retSize, outBuff);
	MemFreeArr(tmpBuff);
	return retSize;
}

UOSInt Crypto::Encrypt::JasyptEncryptor::EncryptAsB64(NN<Text::StringBuilderUTF8> sb, Data::ByteArrayR srcBuff)
{
	UnsafeArrayOpt<UInt8> srcTmpBuff = nullptr;
	UnsafeArray<UInt8> nnsrcTmpBuff;
	UOSInt nBlock = srcBuff.GetSize() / this->ivSize;
	UOSInt destLen;
	if (nBlock * this->ivSize != srcBuff.GetSize())
	{
		destLen = (nBlock + 1) * this->ivSize;
		nnsrcTmpBuff = MemAllocArr(UInt8, destLen);
		MemCopyNO(nnsrcTmpBuff.Ptr(), srcBuff.Arr().Ptr(), srcBuff.GetSize());
		MemFillB(&nnsrcTmpBuff[srcBuff.GetSize()], destLen - srcBuff.GetSize(), (UInt8)(destLen - srcBuff.GetSize()));
		srcBuff = Data::ByteArrayR(nnsrcTmpBuff, destLen);
		srcTmpBuff = nnsrcTmpBuff;
	}
	else
	{
		destLen = nBlock * this->ivSize;		
	}
	if (this->salt.IsNull())
	{
		destLen += this->saltSize;
	}
	if (this->iv.IsNull())
	{
		destLen += this->ivSize;
	}
	UOSInt destOfst = 0;
	UnsafeArray<UInt8> destBuff = MemAllocArr(UInt8, destLen);
	UnsafeArray<const UInt8> salt;
	UnsafeArray<const UInt8> iv;
	if (!UnsafeArrayOpt<const UInt8>(this->salt).SetTo(salt))
	{
		salt = &destBuff[destOfst];
		this->random.NextBytes(&destBuff[destOfst], this->saltSize);
		destOfst += this->saltSize;
	}
	if (!UnsafeArrayOpt<const UInt8>(this->iv).SetTo(iv))
	{
		iv = &destBuff[destOfst];
		this->random.NextBytes(&destBuff[destOfst], this->ivSize);
		destOfst += this->ivSize;
	}
	UnsafeArray<UInt8> key = MemAllocArr(UInt8, this->dkLen);
	this->GetEncKey(salt, key);
	NN<Crypto::Encrypt::Encryption> enc = this->CreateCrypto(iv, key);
	destOfst += enc->Encrypt(srcBuff.Arr().Ptr(), destLen - destOfst, &destBuff[destOfst]);
	enc.Delete();
	MemFreeArr(key);
	if (srcTmpBuff.SetTo(nnsrcTmpBuff))
	{
		MemFreeArr(nnsrcTmpBuff);
	}

	Text::TextBinEnc::Base64Enc b64;
	b64.EncodeBin(sb, destBuff, destOfst);
	MemFreeArr(destBuff);
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
