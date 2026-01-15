#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/FNV1.h"
#include "Text/MyString.h"

extern "C"
{
	UInt32 FNV1_Calc(const UInt8 *buff, UIntOS buffSize, UInt32 currVal);
}

Crypto::Hash::FNV1::FNV1()
{
	this->Clear();
}

Crypto::Hash::FNV1::~FNV1()
{
}

UnsafeArray<UTF8Char> Crypto::Hash::FNV1::GetName(UnsafeArray<UTF8Char> sbuff) const
{
	return Text::StrConcatC(sbuff, UTF8STRC("FNV1"));
}

NN<Crypto::Hash::HashAlgorithm> Crypto::Hash::FNV1::Clone() const
{
	NN<Crypto::Hash::FNV1> fnv1;
	NEW_CLASSNN(fnv1, Crypto::Hash::FNV1());
	fnv1->currVal = this->currVal;
	return fnv1;
}

void Crypto::Hash::FNV1::Clear()
{
	this->currVal = 0x811c9dc5;
}

void Crypto::Hash::FNV1::Calc(UnsafeArray<const UInt8> buff, UIntOS buffSize)
{
	this->currVal = FNV1_Calc(buff.Ptr(), buffSize, this->currVal);
}

void Crypto::Hash::FNV1::GetValue(UnsafeArray<UInt8> buff) const
{
	*(UInt32*)buff.Ptr() = this->currVal;
}

UIntOS Crypto::Hash::FNV1::GetBlockSize() const
{
	return 1;
}

UIntOS Crypto::Hash::FNV1::GetResultSize() const
{
	return 4;
}
