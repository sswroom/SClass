#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/ExcelHash.h"
#include "Data/ByteTool.h"
#include "Text/MyString.h"

Crypto::Hash::ExcelHash::ExcelHash()
{
	this->currVal = 0;
	this->charCnt = 0;
}

Crypto::Hash::ExcelHash::~ExcelHash()
{
}

UTF8Char *Crypto::Hash::ExcelHash::GetName(UTF8Char *sbuff)
{
	return Text::StrConcatC(sbuff, UTF8STRC("Excel Hash"));
}

Crypto::Hash::IHash *Crypto::Hash::ExcelHash::Clone()
{
	Crypto::Hash::ExcelHash *hash;
	NEW_CLASS(hash, Crypto::Hash::ExcelHash());
	hash->currVal = this->currVal;
	hash->charCnt = this->charCnt;
	return hash;
}

void Crypto::Hash::ExcelHash::Clear()
{
	this->currVal = 0;
	this->charCnt = 0;
}

void Crypto::Hash::ExcelHash::Calc(const UInt8 *buff, UOSInt buffSize)
{
	this->charCnt = (UInt16)(this->charCnt + buffSize);
	while (buffSize > 0)
	{
		buffSize--;
		this->currVal = (UInt16)((this->currVal ^ buff[buffSize]) << 1);
		if (this->currVal & 0x8000)
		{
			this->currVal = (this->currVal & 0x7fff) | 1;
		}
	}
}

void Crypto::Hash::ExcelHash::GetValue(UInt8 *buff)
{
	WriteMInt16(buff, this->currVal ^ 0xCE4B ^ this->charCnt);
}

UOSInt Crypto::Hash::ExcelHash::GetBlockSize()
{
	return 1;
}

UOSInt Crypto::Hash::ExcelHash::GetResultSize()
{
	return 2;
}
