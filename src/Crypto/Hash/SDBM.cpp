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

UTF8Char *Crypto::Hash::SDBM::GetName(UTF8Char *sbuff)
{
	return Text::StrConcatC(sbuff, UTF8STRC("SDBM"));
}

Crypto::Hash::IHash *Crypto::Hash::SDBM::Clone()
{
	Crypto::Hash::SDBM *sdbm;
	NEW_CLASS(sdbm, Crypto::Hash::SDBM());
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

void Crypto::Hash::SDBM::GetValue(UInt8 *buff)
{
	*(UInt32*)buff = this->currVal;
}

UOSInt Crypto::Hash::SDBM::GetBlockSize()
{
	return 1;
}

UOSInt Crypto::Hash::SDBM::GetResultSize()
{
	return 4;
}
