#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/SHA1_SHA1.h"
#include "Text/MyString.h"

Crypto::Hash::SHA1_SHA1::SHA1_SHA1(Crypto::Hash::SHA1 *sha1)
{
	this->sha1 = (Crypto::Hash::SHA1*)sha1->Clone();
}

Crypto::Hash::SHA1_SHA1::SHA1_SHA1()
{
	NEW_CLASS(this->sha1, Crypto::Hash::SHA1());
}

Crypto::Hash::SHA1_SHA1::~SHA1_SHA1()
{
	DEL_CLASS(this->sha1);
}

UTF8Char *Crypto::Hash::SHA1_SHA1::GetName(UTF8Char *sbuff)
{
	return Text::StrConcatC(sbuff, UTF8STRC("SHA1_SHA1 (MySQL)"));
}

Crypto::Hash::IHash *Crypto::Hash::SHA1_SHA1::Clone()
{
	Crypto::Hash::IHash *hash;
	NEW_CLASS(hash, Crypto::Hash::SHA1_SHA1(this->sha1));
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

void Crypto::Hash::SHA1_SHA1::GetValue(UInt8 *buff)
{
	UInt8 hashBuff[32];
	Crypto::Hash::SHA1 innerSha1;
	this->sha1->GetValue(hashBuff);
	innerSha1.Calc(hashBuff, this->sha1->GetResultSize());
	innerSha1.GetValue(buff);
}

UOSInt Crypto::Hash::SHA1_SHA1::GetBlockSize()
{
	return this->sha1->GetBlockSize();
}

UOSInt Crypto::Hash::SHA1_SHA1::GetResultSize()
{
	return this->sha1->GetResultSize();
}
