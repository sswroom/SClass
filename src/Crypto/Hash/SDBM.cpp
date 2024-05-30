#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/SDBM.h"
#include "Text/MyString.h"

extern "C"
{
	UInt32 SDBM_Calc(const UInt8 *buff, UOSInt buffSize, UInt32 currVal);
}

Crypto::Hash::SDBM::SDBM()
{
	this->Clear();
}

Crypto::Hash::SDBM::~SDBM()
{
}

UnsafeArray<UTF8Char> Crypto::Hash::SDBM::GetName(UnsafeArray<UTF8Char> sbuff) const
{
	return Text::StrConcatC(sbuff, UTF8STRC("SDBM"));
}

NN<Crypto::Hash::IHash> Crypto::Hash::SDBM::Clone() const
{
	NN<Crypto::Hash::SDBM> sdbm;
	NEW_CLASSNN(sdbm, Crypto::Hash::SDBM());
	sdbm->currVal = this->currVal;
	return sdbm;
}

void Crypto::Hash::SDBM::Clear()
{
	this->currVal = 0;
}

void Crypto::Hash::SDBM::Calc(const UInt8 *buff, UOSInt buffSize)
{
	this->currVal = SDBM_Calc(buff, buffSize, this->currVal);
}

void Crypto::Hash::SDBM::GetValue(UnsafeArray<UInt8> buff) const
{
	*(UInt32*)buff.Ptr() = this->currVal;
}

UOSInt Crypto::Hash::SDBM::GetBlockSize() const
{
	return 1;
}

UOSInt Crypto::Hash::SDBM::GetResultSize() const
{
	return 4;
}
