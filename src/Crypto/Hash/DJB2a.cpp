#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/DJB2a.h"
#include "Text/MyString.h"

extern "C"
{
	UInt32 DJB2a_Calc(const UInt8 *buff, UOSInt buffSize, UInt32 currVal);
}

Crypto::Hash::DJB2a::DJB2a()
{
	this->Clear();
}

Crypto::Hash::DJB2a::~DJB2a()
{
}

UnsafeArray<UTF8Char> Crypto::Hash::DJB2a::GetName(UnsafeArray<UTF8Char> sbuff) const
{
	return Text::StrConcatC(sbuff, UTF8STRC("DJB2a"));
}

NN<Crypto::Hash::HashAlgorithm> Crypto::Hash::DJB2a::Clone() const
{
	NN<Crypto::Hash::DJB2a> djb2;
	NEW_CLASSNN(djb2, Crypto::Hash::DJB2a());
	djb2->currVal = this->currVal;
	return djb2;
}

void Crypto::Hash::DJB2a::Clear()
{
	this->currVal = 5381;
}

void Crypto::Hash::DJB2a::Calc(UnsafeArray<const UInt8> buff, UOSInt buffSize)
{
	this->currVal = DJB2a_Calc(buff.Ptr(), buffSize, this->currVal);
}

void Crypto::Hash::DJB2a::GetValue(UnsafeArray<UInt8> buff) const
{
	*(UInt32*)buff.Ptr() = this->currVal;
}

UOSInt Crypto::Hash::DJB2a::GetBlockSize() const
{
	return 1;
}

UOSInt Crypto::Hash::DJB2a::GetResultSize() const
{
	return 4;
}
