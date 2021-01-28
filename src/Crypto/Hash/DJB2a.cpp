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

UTF8Char *Crypto::Hash::DJB2a::GetName(UTF8Char *sbuff)
{
	return Text::StrConcat(sbuff, (const UTF8Char*)"DJB2a");
}

Crypto::Hash::IHash *Crypto::Hash::DJB2a::Clone()
{
	Crypto::Hash::DJB2a *djb2;
	NEW_CLASS(djb2, Crypto::Hash::DJB2a());
	djb2->currVal = this->currVal;
	return djb2;
}

void Crypto::Hash::DJB2a::Clear()
{
	this->currVal = 5381;
}

void Crypto::Hash::DJB2a::Calc(const UInt8 *buff, UOSInt buffSize)
{
	this->currVal = DJB2a_Calc(buff, buffSize, this->currVal);
}

void Crypto::Hash::DJB2a::GetValue(UInt8 *buff)
{
	*(UInt32*)buff = this->currVal;
}

UOSInt Crypto::Hash::DJB2a::GetBlockSize()
{
	return 1;
}

UOSInt Crypto::Hash::DJB2a::GetResultSize()
{
	return 4;
}
