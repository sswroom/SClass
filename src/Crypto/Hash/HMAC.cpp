#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/HMAC.h"
#include "Text/MyString.h"

Crypto::Hash::HMAC::HMAC(Crypto::Hash::IHash *hash, const UInt8 *key, UOSInt keySize)
{
	this->hashInner = hash->Clone();
	this->hashOuter = hash->Clone();
	this->key = MemAlloc(UInt8, keySize);
	this->keySize = keySize;
	MemCopyNO(this->key, key, keySize);

	this->padSize = hash->GetBlockSize();
	if (this->padSize < keySize)
		this->padSize = keySize;
	this->iPad = MemAlloc(UInt8, this->padSize);
	this->oPad = MemAlloc(UInt8, this->padSize);
	UOSInt i = this->padSize;
	while (i-- > 0)
	{
		this->iPad[i] = 0x36;
		this->oPad[i] = 0x5c;
	}
	
	i = keySize;
	while (i-- > 0)
	{
		UInt8 b = key[i];
		this->iPad[i] ^= b;
		this->oPad[i] ^= b;
	}

	this->Clear();
}

Crypto::Hash::HMAC::~HMAC()
{
	DEL_CLASS(hashInner);
	DEL_CLASS(hashOuter);
	if (this->key)
	{
		MemFree(key);
		this->key = 0;
	}
	if (this->iPad)
	{
		MemFree(iPad);
		this->iPad = 0;
	}
	if (this->oPad)
	{
		MemFree(oPad);
		this->oPad = 0;
	}
}

UTF8Char *Crypto::Hash::HMAC::GetName(UTF8Char *sbuff)
{
	return this->hashInner->GetName(Text::StrConcat(sbuff, (const UTF8Char*)"HMAC-"));
}

Crypto::Hash::IHash *Crypto::Hash::HMAC::Clone()
{
	Crypto::Hash::IHash *hmac;
	NEW_CLASS(hmac, Crypto::Hash::HMAC(this->hashInner, key, keySize));
	return hmac;
}

void Crypto::Hash::HMAC::Clear()
{
	this->hashInner->Clear();
	this->hashInner->Calc(iPad, padSize);
}

void Crypto::Hash::HMAC::Calc(const UInt8 *buff, UOSInt buffSize)
{
	this->hashInner->Calc(buff, buffSize);
}

void Crypto::Hash::HMAC::GetValue(UInt8 *buff)
{
	this->hashInner->GetValue(buff);
	this->hashOuter->Clear();
	this->hashOuter->Calc(oPad, padSize);
	this->hashOuter->Calc(buff, hashInner->GetResultSize());
	this->hashOuter->GetValue(buff);
}

UOSInt Crypto::Hash::HMAC::GetBlockSize()
{
	return this->hashInner->GetBlockSize();
}

UOSInt Crypto::Hash::HMAC::GetResultSize()
{
	return this->hashInner->GetResultSize();
}
