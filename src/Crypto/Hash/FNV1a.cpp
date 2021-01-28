#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/FNV1a.h"
#include "Text/MyString.h"

extern "C"
{
	UInt32 FNV1a_Calc(const UInt8 *buff, UOSInt buffSize, UInt32 currVal);
}

Crypto::Hash::FNV1a::FNV1a()
{
	this->Clear();
}

Crypto::Hash::FNV1a::~FNV1a()
{
}

UTF8Char *Crypto::Hash::FNV1a::GetName(UTF8Char *sbuff)
{
	return Text::StrConcat(sbuff, (const UTF8Char*)"FNV1a");
}

Crypto::Hash::IHash *Crypto::Hash::FNV1a::Clone()
{
	Crypto::Hash::FNV1a *fnv1;
	NEW_CLASS(fnv1, Crypto::Hash::FNV1a());
	fnv1->currVal = this->currVal;
	return fnv1;
}

void Crypto::Hash::FNV1a::Clear()
{
	this->currVal = 0x811c9dc5;
}

void Crypto::Hash::FNV1a::Calc(const UInt8 *buff, UOSInt buffSize)
{
	this->currVal = FNV1a_Calc(buff, buffSize, this->currVal);
}

void Crypto::Hash::FNV1a::GetValue(UInt8 *buff)
{
	*(UInt32*)buff = this->currVal;
}

UOSInt Crypto::Hash::FNV1a::GetBlockSize()
{
	return 1;
}

UOSInt Crypto::Hash::FNV1a::GetResultSize()
{
	return 4;
}
