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

UnsafeArray<UTF8Char> Crypto::Hash::FNV1a::GetName(UnsafeArray<UTF8Char> sbuff) const
{
	return Text::StrConcatC(sbuff, UTF8STRC("FNV1a"));
}

NN<Crypto::Hash::HashAlgorithm> Crypto::Hash::FNV1a::Clone() const
{
	NN<Crypto::Hash::FNV1a> fnv1;
	NEW_CLASSNN(fnv1, Crypto::Hash::FNV1a());
	fnv1->currVal = this->currVal;
	return fnv1;
}

void Crypto::Hash::FNV1a::Clear()
{
	this->currVal = 0x811c9dc5;
}

void Crypto::Hash::FNV1a::Calc(UnsafeArray<const UInt8> buff, UOSInt buffSize)
{
	this->currVal = FNV1a_Calc(buff.Ptr(), buffSize, this->currVal);
}

void Crypto::Hash::FNV1a::GetValue(UnsafeArray<UInt8> buff) const
{
	*(UInt32*)buff.Ptr() = this->currVal;
}

UOSInt Crypto::Hash::FNV1a::GetBlockSize() const
{
	return 1;
}

UOSInt Crypto::Hash::FNV1a::GetResultSize() const
{
	return 4;
}
