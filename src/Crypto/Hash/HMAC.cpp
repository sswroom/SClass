#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/HMAC.h"
#include "Text/MyString.h"

Crypto::Hash::HMAC::HMAC(NN<Crypto::Hash::HashAlgorithm> hash, UnsafeArray<const UInt8> key, UIntOS keySize)
{
	this->hashInner = hash->Clone();
	this->hashOuter = hash->Clone();
	this->key = MemAllocArr(UInt8, keySize);
	this->keySize = keySize;
	MemCopyNO(this->key.Ptr(), key.Ptr(), keySize);

	this->padSize = hash->GetBlockSize();
	if (this->padSize < keySize)
		this->padSize = keySize;
	this->iPad = MemAllocArr(UInt8, this->padSize);
	this->oPad = MemAllocArr(UInt8, this->padSize);
	UIntOS i = this->padSize;
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
	this->hashInner.Delete();
	this->hashOuter.Delete();
	MemFreeArr(this->key);
	MemFreeArr(this->iPad);
	MemFreeArr(this->oPad);
}

UnsafeArray<UTF8Char> Crypto::Hash::HMAC::GetName(UnsafeArray<UTF8Char> sbuff) const
{
	return this->hashInner->GetName(Text::StrConcatC(sbuff, UTF8STRC("HMAC-")));
}

NN<Crypto::Hash::HashAlgorithm> Crypto::Hash::HMAC::Clone() const
{
	NN<Crypto::Hash::HashAlgorithm> hmac;
	NEW_CLASSNN(hmac, Crypto::Hash::HMAC(this->hashInner, key, keySize));
	return hmac;
}

void Crypto::Hash::HMAC::Clear()
{
	this->hashInner->Clear();
	this->hashInner->Calc(iPad, padSize);
}

void Crypto::Hash::HMAC::Calc(UnsafeArray<const UInt8> buff, UIntOS buffSize)
{
	this->hashInner->Calc(buff, buffSize);
}

void Crypto::Hash::HMAC::GetValue(UnsafeArray<UInt8> buff) const
{
	this->hashInner->GetValue(buff);
	this->hashOuter->Clear();
	this->hashOuter->Calc(oPad, padSize);
	this->hashOuter->Calc(buff.Ptr(), hashInner->GetResultSize());
	this->hashOuter->GetValue(buff);
}

UIntOS Crypto::Hash::HMAC::GetBlockSize() const
{
	return this->hashInner->GetBlockSize();
}

UIntOS Crypto::Hash::HMAC::GetResultSize() const
{
	return this->hashInner->GetResultSize();
}
