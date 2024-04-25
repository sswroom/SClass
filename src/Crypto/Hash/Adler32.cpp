#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/Adler32.h"
#include "Data/ByteTool.h"
#include "Text/MyString.h"

extern "C"
{
	UInt32 Adler32_Calc(const UInt8 *buff, UOSInt buffSize, UInt32 abVal);
}

Crypto::Hash::Adler32::Adler32(const Adler32 *adler32)
{
	this->abVal = adler32->abVal;
}

Crypto::Hash::Adler32::Adler32()
{
	this->abVal = 1;
}

Crypto::Hash::Adler32::~Adler32()
{
}

UTF8Char *Crypto::Hash::Adler32::GetName(UTF8Char *sbuff) const
{
	return Text::StrConcatC(sbuff, UTF8STRC("Adler-32"));
}

NN<Crypto::Hash::IHash> Crypto::Hash::Adler32::Clone() const
{
	NN<Crypto::Hash::Adler32> adler32;
	NEW_CLASSNN(adler32, Crypto::Hash::Adler32(this));
	return adler32;
}

void Crypto::Hash::Adler32::Clear()
{
	this->abVal = 1;
}

void Crypto::Hash::Adler32::Calc(const UInt8 *buff, UOSInt buffSize)
{
	this->abVal = Adler32_Calc(buff, buffSize, this->abVal);
}

void Crypto::Hash::Adler32::GetValue(UInt8 *buff) const
{
	WriteMUInt32(buff, this->abVal);
}

UOSInt Crypto::Hash::Adler32::GetBlockSize() const
{
	return 1;
}

UOSInt Crypto::Hash::Adler32::GetResultSize() const
{
	return 4;
}
