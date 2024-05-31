#include "Stdafx.h"
#include "MemTool.h"
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

UnsafeArray<UTF8Char> Crypto::Hash::CRC32::GetName(UnsafeArray<UTF8Char> sbuff) const
{
	return Text::StrConcatC(sbuff, UTF8STRC("CRC (32-bit)"));
}

NN<Crypto::Hash::IHash> Crypto::Hash::CRC32::Clone() const
{
	NN<Crypto::Hash::CRC32> crc;
	NEW_CLASSNN(crc, Crypto::Hash::CRC32(this));
	return crc;
}

void Crypto::Hash::CRC32::Clear()
{
	currVal = 0;
}

void Crypto::Hash::CRC32::Calc(UnsafeArray<const UInt8> buff, UOSInt buffSize)
{
	this->currVal = CRC32_Calc(buff.Ptr(), buffSize, this->crctab, this->currVal);
}

void Crypto::Hash::CRC32::GetValue(UnsafeArray<UInt8> buff) const
{
	WriteMUInt32(buff.Ptr(), currVal);
}

UOSInt Crypto::Hash::CRC32::GetBlockSize() const
{
	return 1;
}

UOSInt Crypto::Hash::CRC32::GetResultSize() const
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
