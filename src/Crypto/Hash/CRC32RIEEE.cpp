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

UTF8Char *Crypto::Hash::CRC32RIEEE::GetName(UTF8Char *sbuff)
{
	return Text::StrConcat(sbuff, (const UTF8Char*)"CRC32 IEEE");
}

Crypto::Hash::IHash *Crypto::Hash::CRC32RIEEE::Clone()
{
	Crypto::Hash::CRC32RIEEE *crc;
	NEW_CLASS(crc, Crypto::Hash::CRC32RIEEE());
	crc->currVal = this->currVal;
	return crc;
}

void Crypto::Hash::CRC32RIEEE::Clear()
{
	currVal = 0xffffffff;
}

void Crypto::Hash::CRC32RIEEE::Calc(const UInt8 *buff, UOSInt buffSize)
{
	this->currVal = CRC32R_Calc(buff, buffSize, crctab, this->currVal);
}

void Crypto::Hash::CRC32RIEEE::GetValue(UInt8 *buff)
{
	WriteMUInt32(buff, ~currVal);
}

UOSInt Crypto::Hash::CRC32RIEEE::GetBlockSize()
{
	return 1;
}

UOSInt Crypto::Hash::CRC32RIEEE::GetResultSize()
{
	return 4;
}

UInt32 Crypto::Hash::CRC32RIEEE::GetValueU32()
{
	return ~this->currVal;
}
