#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/Adler32.h"
#include "Core/ByteTool_C.h"
#include "Text/MyString.h"

Crypto::Hash::Adler32::Adler32(NN<const Adler32> adler32)
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

UnsafeArray<UTF8Char> Crypto::Hash::Adler32::GetName(UnsafeArray<UTF8Char> sbuff) const
{
	return Text::StrConcatC(sbuff, UTF8STRC("Adler-32"));
}

NN<Crypto::Hash::HashAlgorithm> Crypto::Hash::Adler32::Clone() const
{
	NN<Crypto::Hash::Adler32> adler32;
	NEW_CLASSNN(adler32, Crypto::Hash::Adler32(*this));
	return adler32;
}

void Crypto::Hash::Adler32::Clear()
{
	this->abVal = 1;
}

void Crypto::Hash::Adler32::Calc(UnsafeArray<const UInt8> buff, UIntOS buffSize)
{
	this->abVal = Adler32_Calc(buff.Ptr(), buffSize, this->abVal);
}

void Crypto::Hash::Adler32::GetValue(UnsafeArray<UInt8> buff) const
{
	WriteMUInt32(buff.Ptr(), this->abVal);
}

UIntOS Crypto::Hash::Adler32::GetBlockSize() const
{
	return 1;
}

UIntOS Crypto::Hash::Adler32::GetResultSize() const
{
	return 4;
}
