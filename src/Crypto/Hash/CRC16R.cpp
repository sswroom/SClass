#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/CRC16R.h"
#include "Data/ByteTool.h"
#include "Text/MyString.h"

extern "C"
{
	void CRC16R_InitTable(UInt16 *tab, UInt16 rpn);
	UInt16 CRC16R_Calc(const UInt8 *buff, UOSInt buffSize, UInt16 *tab, UInt16 currVal);
}

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

UTF8Char *Crypto::Hash::CRC16R::GetName(UTF8Char *sbuff)
{
	return Text::StrConcat(sbuff, (const UTF8Char*)"CRC (16-bit Reversed)");
}

Crypto::Hash::IHash *Crypto::Hash::CRC16R::Clone()
{
	Crypto::Hash::CRC16R *crc;
	NEW_CLASS(crc, Crypto::Hash::CRC16R(this->polynomial));
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

void Crypto::Hash::CRC16R::GetValue(UInt8 *buff)
{
	WriteMInt16(buff, ~currVal);
}

UOSInt Crypto::Hash::CRC16R::GetBlockSize()
{
	return 1;
}

UOSInt Crypto::Hash::CRC16R::GetResultSize()
{
	return 2;
}

UInt16 Crypto::Hash::CRC16R::Reverse(UInt16 polynomial)
{
	UInt16 v;
	UInt16 v2;
	OSInt i = 16;
	v = polynomial;
	v2 = 0;
	while (i-- > 0)
	{
		v2 = (v2 >> 1) | (v & 0x8000);
		v <<= 1;
	}
	return v2;
}
