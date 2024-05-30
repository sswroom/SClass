#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/SHA1_SHA1.h"
#include "Text/MyString.h"

Crypto::Hash::SHA1_SHA1::SHA1_SHA1(NN<Crypto::Hash::SHA1> sha1)
{
	this->sha1 = NN<Crypto::Hash::SHA1>::ConvertFrom(sha1->Clone());
}

Crypto::Hash::SHA1_SHA1::SHA1_SHA1()
{
	NEW_CLASSNN(this->sha1, Crypto::Hash::SHA1());
}

Crypto::Hash::SHA1_SHA1::~SHA1_SHA1()
{
	this->sha1.Delete();
}

UnsafeArray<UTF8Char> Crypto::Hash::SHA1_SHA1::GetName(UnsafeArray<UTF8Char> sbuff) const
{
	return Text::StrConcatC(sbuff, UTF8STRC("SHA1_SHA1 (MySQL)"));
}

NN<Crypto::Hash::IHash> Crypto::Hash::SHA1_SHA1::Clone() const
{
	NN<Crypto::Hash::IHash> hash;
	NEW_CLASSNN(hash, Crypto::Hash::SHA1_SHA1(this->sha1));
	return hash;
}

void Crypto::Hash::SHA1_SHA1::Clear()
{
	this->sha1->Clear();
}

void Crypto::Hash::SHA1_SHA1::Calc(const UInt8 *buff, UOSInt buffSize)
{
	this->sha1->Calc(buff, buffSize);
}

void Crypto::Hash::SHA1_SHA1::GetValue(UnsafeArray<UInt8> buff) const
{
	UInt8 hashBuff[32];
	Crypto::Hash::SHA1 innerSha1;
	this->sha1->GetValue(hashBuff);
	innerSha1.Calc(hashBuff, this->sha1->GetResultSize());
	innerSha1.GetValue(buff);
}

UOSInt Crypto::Hash::SHA1_SHA1::GetBlockSize() const
{
	return this->sha1->GetBlockSize();
}

UOSInt Crypto::Hash::SHA1_SHA1::GetResultSize() const
{
	return this->sha1->GetResultSize();
}
