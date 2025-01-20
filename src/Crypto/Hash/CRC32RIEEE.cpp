#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/CRC32.h"
#include "Crypto/Hash/CRC32RIEEE.h"
#include "Data/ByteTool.h"
#include "Text/MyString.h"

extern "C"
{
	void CRC32R_InitTable(UInt32 *tab, UInt32 rpn);
	UInt32 CRC32R_Calc(const UInt8 *buff, UOSInt buffSize, UInt32 *tab, UInt32 currVal);
}

Bool Crypto::Hash::CRC32RIEEE::tabInited = false;
UInt32 Crypto::Hash::CRC32RIEEE::crctab[4096];

Crypto::Hash::CRC32RIEEE::CRC32RIEEE()
{
	this->currVal = 0xffffffff;
	if (!tabInited)
	{
		tabInited = true;
		UInt32 rpn = CRC32R_Reverse(Crypto::Hash::CRC32::GetPolynormialIEEE());
		CRC32R_InitTable(crctab, rpn);
	}
}

Crypto::Hash::CRC32RIEEE::~CRC32RIEEE()
{
}

UnsafeArray<UTF8Char> Crypto::Hash::CRC32RIEEE::GetName(UnsafeArray<UTF8Char> sbuff) const
{
	return Text::StrConcatC(sbuff, UTF8STRC("CRC32 IEEE"));
}

NN<Crypto::Hash::HashAlgorithm> Crypto::Hash::CRC32RIEEE::Clone() const
{
	NN<Crypto::Hash::CRC32RIEEE> crc;
	NEW_CLASSNN(crc, Crypto::Hash::CRC32RIEEE());
	crc->currVal = this->currVal;
	return crc;
}

void Crypto::Hash::CRC32RIEEE::Clear()
{
	currVal = 0xffffffff;
}

void Crypto::Hash::CRC32RIEEE::Calc(UnsafeArray<const UInt8> buff, UOSInt buffSize)
{
	this->currVal = CRC32R_Calc(buff.Ptr(), buffSize, crctab, this->currVal);
}

void Crypto::Hash::CRC32RIEEE::GetValue(UnsafeArray<UInt8> buff) const
{
	WriteMUInt32(buff.Ptr(), ~currVal);
}

UOSInt Crypto::Hash::CRC32RIEEE::GetBlockSize() const
{
	return 1;
}

UOSInt Crypto::Hash::CRC32RIEEE::GetResultSize() const
{
	return 4;
}

UInt32 Crypto::Hash::CRC32RIEEE::GetValueU32() const
{
	return ~this->currVal;
}
