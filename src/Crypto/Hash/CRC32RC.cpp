#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/CRC32.h"
#include "Crypto/Hash/CRC32RC.h"
#include "Data/ByteTool.h"
#include "Text/MyString.h"

extern "C"
{
	void CRC32R_InitTable(UInt32 *tab, UInt32 rpn);
}

Bool Crypto::Hash::CRC32RC::tabInited = false;
UInt32 Crypto::Hash::CRC32RC::crctab[4096];

Crypto::Hash::CRC32RC::CRC32RC()
{
	this->currVal = 0xffffffff;
	if (!tabInited)
	{
		tabInited = true;
		UInt32 rpn = CRC32R_Reverse(Crypto::Hash::CRC32::GetPolynormialCastagnoli());
		CRC32R_InitTable(crctab, rpn);
	}
}

Crypto::Hash::CRC32RC::~CRC32RC()
{
}

UnsafeArray<UTF8Char> Crypto::Hash::CRC32RC::GetName(UnsafeArray<UTF8Char> sbuff) const
{
	return Text::StrConcatC(sbuff, UTF8STRC("CRC-32C"));
}

NN<Crypto::Hash::HashAlgorithm> Crypto::Hash::CRC32RC::Clone() const
{
	NN<Crypto::Hash::CRC32RC> crc;
	NEW_CLASSNN(crc, Crypto::Hash::CRC32RC());
	crc->currVal = this->currVal;
	return crc;
}

void Crypto::Hash::CRC32RC::Clear()
{
	currVal = 0xffffffff;
}

void Crypto::Hash::CRC32RC::Calc(UnsafeArray<const UInt8> buff, UOSInt buffSize)
{
	this->currVal = CRC32R_Calc(buff.Ptr(), buffSize, crctab, this->currVal);
}

void Crypto::Hash::CRC32RC::GetValue(UnsafeArray<UInt8> buff) const
{
	WriteMUInt32(buff.Ptr(), ~currVal);
}

UOSInt Crypto::Hash::CRC32RC::GetBlockSize() const
{
	return 1;
}

UOSInt Crypto::Hash::CRC32RC::GetResultSize() const
{
	return 4;
}

UInt32 Crypto::Hash::CRC32RC::GetValueU32() const
{
	return ~this->currVal;
}
