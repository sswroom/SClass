#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/CRC32.h"
#include "Crypto/Hash/CRC32R.h"
#include "Core/ByteTool_C.h"
#include "Text/MyString.h"

extern "C"
{
	void CRC32R_InitTable(UInt32 *tab, UInt32 rpn);
	UInt32 CRC32R_Calc(const UInt8 *buff, UOSInt buffSize, UInt32 *tab, UInt32 currVal);
}

Crypto::Hash::CRC32R::CRC32R(NN<const CRC32R> crc)
{
	this->crctab = MemAllocArr(UInt32, 256*16);
	this->currVal = crc->currVal;
	MemCopyNO(this->crctab.Ptr(), crc->crctab.Ptr(), sizeof(UInt32) * 256*16);
}

void Crypto::Hash::CRC32R::InitTable(UInt32 polynomial)
{
	currVal = 0xffffffff;

	UInt32 rpn = CRC32R_Reverse(polynomial);
	UnsafeArray<UInt32> tab = crctab = MemAllocArr(UInt32, 256*16);
	CRC32R_InitTable(tab.Ptr(), rpn);
}

Crypto::Hash::CRC32R::CRC32R()
{
	InitTable(Crypto::Hash::CRC32::GetPolynormialIEEE());
}

Crypto::Hash::CRC32R::CRC32R(UInt32 polynomial)
{
	InitTable(polynomial);
}

Crypto::Hash::CRC32R::~CRC32R()
{
	MemFreeArr(crctab);
}

UnsafeArray<UTF8Char> Crypto::Hash::CRC32R::GetName(UnsafeArray<UTF8Char> sbuff) const
{
	return Text::StrConcatC(sbuff, UTF8STRC("CRC (32-bit Reversed)"));
}

NN<Crypto::Hash::HashAlgorithm> Crypto::Hash::CRC32R::Clone() const
{
	NN<Crypto::Hash::CRC32R> crc;
	NEW_CLASSNN(crc, Crypto::Hash::CRC32R(*this));
	return crc;
}

void Crypto::Hash::CRC32R::Clear()
{
	currVal = 0xffffffff;
}

void Crypto::Hash::CRC32R::Calc(UnsafeArray<const UInt8> buff, UOSInt buffSize)
{
	this->currVal = CRC32R_Calc(buff.Ptr(), buffSize, this->crctab.Ptr(), this->currVal);
}

void Crypto::Hash::CRC32R::GetValue(UnsafeArray<UInt8> buff) const
{
	WriteMUInt32(buff.Ptr(), ~currVal);
}

UOSInt Crypto::Hash::CRC32R::GetBlockSize() const
{
	return 1;
}

UOSInt Crypto::Hash::CRC32R::GetResultSize() const
{
	return 4;
}
