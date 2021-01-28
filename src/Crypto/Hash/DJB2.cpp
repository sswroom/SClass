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

UTF8Char *Crypto::Hash::DJB2::GetName(UTF8Char *sbuff)
{
	return Text::StrConcat(sbuff, (const UTF8Char*)"DJB2");
}

Crypto::Hash::IHash *Crypto::Hash::DJB2::Clone()
{
	Crypto::Hash::DJB2 *djb2;
	NEW_CLASS(djb2, Crypto::Hash::DJB2());
	djb2->currVal = this->currVal;
	return djb2;
}

void Crypto::Hash::DJB2::Clear()
{
	this->currVal = 5381;
}

void Crypto::Hash::DJB2::Calc(const UInt8 *buff, UOSInt buffSize)
{
	this->currVal = DJB2_Calc(buff, buffSize, this->currVal);
}

void Crypto::Hash::DJB2::GetValue(UInt8 *buff)
{
	*(UInt32*)buff = this->currVal;
}

UOSInt Crypto::Hash::DJB2::GetBlockSize()
{
	return 1;
}

UOSInt Crypto::Hash::DJB2::GetResultSize()
{
	return 4;
}
