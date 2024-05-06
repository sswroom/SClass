#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/CRC16.h"
#include "Data/ByteTool.h"
#include "Text/MyString.h"

void Crypto::Hash::CRC16::BuildTable(UInt16 polynomial)
{
	this->currVal = 0;

	this->polynomial = polynomial;
	UInt16 *tab = crctab = MemAlloc(UInt16, 256 * 16);
	CRC16_InitTable(tab, polynomial);
}

Crypto::Hash::CRC16::CRC16(UInt16 polynomial)
{
	BuildTable(polynomial);
}

Crypto::Hash::CRC16::CRC16()
{
	BuildTable(GetPolynomialCCITT());
}

Crypto::Hash::CRC16::~CRC16()
{
	MemFree(crctab);
}

UTF8Char *Crypto::Hash::CRC16::GetName(UTF8Char *sbuff) const
{
	return Text::StrConcatC(sbuff, UTF8STRC("CRC (16-bit)"));
}

NN<Crypto::Hash::IHash> Crypto::Hash::CRC16::Clone() const
{
	NN<Crypto::Hash::CRC16> crc;
	NEW_CLASSNN(crc, Crypto::Hash::CRC16(this->polynomial));
	crc->currVal = this->currVal;
	return crc;
}

void Crypto::Hash::CRC16::Clear()
{
	currVal = 0;
}

void Crypto::Hash::CRC16::Calc(const UInt8 *buff, UOSInt buffSize)
{
	this->currVal = CRC16_Calc(buff, buffSize, this->crctab, this->currVal);
}

void Crypto::Hash::CRC16::GetValue(UnsafeArray<UInt8> buff) const
{
	WriteMInt16(buff, currVal);
}

UOSInt Crypto::Hash::CRC16::GetBlockSize() const
{
	return 1;
}

UOSInt Crypto::Hash::CRC16::GetResultSize() const
{
	return 2;
}

void Crypto::Hash::CRC16::SetValue(UInt16 val)
{
	this->currVal = val;
}

UInt16 Crypto::Hash::CRC16::GetPolynomialANSI()
{
	return 0x8005;
}

UInt16 Crypto::Hash::CRC16::GetPolynomialCCITT()
{
	return 0x1021;
}

UInt16 Crypto::Hash::CRC16::GetPolynomialT10_DIF()
{
	return 0x8BB7;
}

UInt16 Crypto::Hash::CRC16::GetPolynomialDNP()
{
	return 0x3D65;
}

UInt16 Crypto::Hash::CRC16::GetPolynomialDECT()
{
	return 0x0589;
}

UInt16 Crypto::Hash::CRC16::GetPolynomialARINC()
{
	return 0xA02B;
}
