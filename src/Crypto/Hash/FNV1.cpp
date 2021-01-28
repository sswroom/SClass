#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/FNV1.h"
#include "Text/MyString.h"

extern "C"
{
	UInt32 FNV1_Calc(const UInt8 *buff, UOSInt buffSize, UInt32 currVal);
}

Crypto::Hash::FNV1::FNV1()
{
	this->Clear();
}

Crypto::Hash::FNV1::~FNV1()
{
}

UTF8Char *Crypto::Hash::FNV1::GetName(UTF8Char *sbuff)
{
	return Text::StrConcat(sbuff, (const UTF8Char*)"FNV1");
}

Crypto::Hash::IHash *Crypto::Hash::FNV1::Clone()
{
	Crypto::Hash::FNV1 *fnv1;
	NEW_CLASS(fnv1, Crypto::Hash::FNV1());
	fnv1->currVal = this->currVal;
	return fnv1;
}

void Crypto::Hash::FNV1::Clear()
{
	this->currVal = 0x811c9dc5;
}

void Crypto::Hash::FNV1::Calc(const UInt8 *buff, UOSInt buffSize)
{
	this->currVal = FNV1_Calc(buff, buffSize, this->currVal);
}

void Crypto::Hash::FNV1::GetValue(UInt8 *buff)
{
	*(UInt32*)buff = this->currVal;
}

UOSInt Crypto::Hash::FNV1::GetBlockSize()
{
	return 1;
}

UOSInt Crypto::Hash::FNV1::GetResultSize()
{
	return 4;
}
