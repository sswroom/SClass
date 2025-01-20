#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/DJB2.h"
#include "Text/MyString.h"

extern "C"
{
	UInt32 DJB2_Calc(const UInt8 *buff, UOSInt buffSize, UInt32 currVal);
}

Crypto::Hash::DJB2::DJB2()
{
	this->Clear();
}

Crypto::Hash::DJB2::~DJB2()
{
}

UnsafeArray<UTF8Char> Crypto::Hash::DJB2::GetName(UnsafeArray<UTF8Char> sbuff) const
{
	return Text::StrConcatC(sbuff, UTF8STRC("DJB2"));
}

NN<Crypto::Hash::HashAlgorithm> Crypto::Hash::DJB2::Clone() const
{
	NN<Crypto::Hash::DJB2> djb2;
	NEW_CLASSNN(djb2, Crypto::Hash::DJB2());
	djb2->currVal = this->currVal;
	return djb2;
}

void Crypto::Hash::DJB2::Clear()
{
	this->currVal = 5381;
}

void Crypto::Hash::DJB2::Calc(UnsafeArray<const UInt8> buff, UOSInt buffSize)
{
	this->currVal = DJB2_Calc(buff.Ptr(), buffSize, this->currVal);
}

void Crypto::Hash::DJB2::GetValue(UnsafeArray<UInt8> buff) const
{
	*(UInt32*)buff.Ptr() = this->currVal;
}

UOSInt Crypto::Hash::DJB2::GetBlockSize() const
{
	return 1;
}

UOSInt Crypto::Hash::DJB2::GetResultSize() const
{
	return 4;
}
