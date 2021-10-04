#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/CRC32.h"
#include "Data/ByteTool.h"
#include "Text/MyString.h"

extern "C"
{
	void CRC32_InitTable(UInt32 *tab, UInt32 polynomial);
	UInt32 CRC32_Calc(const UInt8 *buff, UOSInt buffSize, UInt32 *tab, UInt32 currVal);
}

Crypto::Hash::CRC32::CRC32(const CRC32 *crc)
{
	this->crctab = MemAlloc(UInt32, 256);
	this->currVal = crc->currVal;
	MemCopyNO(this->crctab, crc->crctab, sizeof(UInt32) * 256 * 16);
}

void Crypto::Hash::CRC32::InitTable(UInt32 polynomial)
{
	this->currVal = 0;

	UInt32 *tab = this->crctab = MemAlloc(UInt32, 256 * 16);
	CRC32_InitTable(tab, polynomial);
}

Crypto::Hash::CRC32::CRC32()
{
	InitTable(GetPolynormialIEEE());
}

Crypto::Hash::CRC32::CRC32(UInt32 polynomial)
{
	InitTable(polynomial);
}

Crypto::Hash::CRC32::~CRC32()
{
	MemFree(crctab);
}

UTF8Char *Crypto::Hash::CRC32::GetName(UTF8Char *sbuff)
{
	return Text::StrConcat(sbuff, (const UTF8Char*)"CRC (32-bit)");
}

Crypto::Hash::IHash *Crypto::Hash::CRC32::Clone()
{
	Crypto::Hash::CRC32 *crc;
	NEW_CLASS(crc, Crypto::Hash::CRC32(this));
	return crc;
}

void Crypto::Hash::CRC32::Clear()
{
	currVal = 0;
}

void Crypto::Hash::CRC32::Calc(const UInt8 *buff, UOSInt buffSize)
{
	this->currVal = CRC32_Calc(buff, buffSize, this->crctab, this->currVal);
}

void Crypto::Hash::CRC32::GetValue(UInt8 *buff)
{
	WriteMUInt32(buff, currVal);
}

UOSInt Crypto::Hash::CRC32::GetBlockSize()
{
	return 1;
}

UOSInt Crypto::Hash::CRC32::GetResultSize()
{
	return 4;
}

UInt32 Crypto::Hash::CRC32::GetPolynormialIEEE()
{
	return 0x04C11DB7;
}

UInt32 Crypto::Hash::CRC32::GetPolynormialCastagnoli()
{
	return 0x1EDC6F41;
}
