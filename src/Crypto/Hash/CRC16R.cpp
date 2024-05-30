#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/CRC16R.h"
#include "Data/ByteTool.h"
#include "Text/MyString.h"

void Crypto::Hash::CRC16R::BuildTable(UInt16 polynomial)
{
	currVal = 0xffff;

	this->polynomial = polynomial;
	UInt16 rpn = Reverse(polynomial);
	UInt16 *tab = crctab = MemAlloc(UInt16, 256 * 16);
	CRC16R_InitTable(tab, rpn);
}

Crypto::Hash::CRC16R::CRC16R(UInt16 polynomial)
{
	BuildTable(polynomial);
}

Crypto::Hash::CRC16R::CRC16R()
{
	BuildTable(0x8005);
}

Crypto::Hash::CRC16R::~CRC16R()
{
	MemFree(crctab);
}

UnsafeArray<UTF8Char> Crypto::Hash::CRC16R::GetName(UnsafeArray<UTF8Char> sbuff) const
{
	return Text::StrConcatC(sbuff, UTF8STRC("CRC (16-bit Reversed)"));
}

NN<Crypto::Hash::IHash> Crypto::Hash::CRC16R::Clone() const
{
	NN<Crypto::Hash::CRC16R> crc;
	NEW_CLASSNN(crc, Crypto::Hash::CRC16R(this->polynomial));
	return crc;
}

void Crypto::Hash::CRC16R::Clear()
{
	currVal = 0xffff;
}

void Crypto::Hash::CRC16R::Calc(const UInt8 *buff, UOSInt buffSize)
{
	this->currVal = CRC16R_Calc(buff, buffSize, this->crctab, this->currVal);;
}

void Crypto::Hash::CRC16R::GetValue(UnsafeArray<UInt8> buff) const
{
	WriteMInt16(buff.Ptr(), ~currVal);
}

UOSInt Crypto::Hash::CRC16R::GetBlockSize() const
{
	return 1;
}

UOSInt Crypto::Hash::CRC16R::GetResultSize() const
{
	return 2;
}

UInt16 Crypto::Hash::CRC16R::Reverse(UInt16 polynomial)
{
	UInt16 v;
	UInt16 v2;
	UOSInt i = 16;
	v = polynomial;
	v2 = 0;
	while (i-- > 0)
	{
		v2 = (UInt16)((v2 >> 1) | (v & 0x8000));
		v = (UInt16)(v << 1);
	}
	return v2;
}
