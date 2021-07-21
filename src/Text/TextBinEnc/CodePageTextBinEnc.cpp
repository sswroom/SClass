#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/TextBinEnc/CodePageTextBinEnc.h"

Text::TextBinEnc::CodePageTextBinEnc::CodePageTextBinEnc(UInt32 codePage)
{
	NEW_CLASS(this->enc, Text::Encoding(codePage));
}

Text::TextBinEnc::CodePageTextBinEnc::~CodePageTextBinEnc()
{
	DEL_CLASS(this->enc);
}

UOSInt Text::TextBinEnc::CodePageTextBinEnc::EncodeBin(Text::StringBuilderUTF *sb, const UInt8 *dataBuff, UOSInt buffSize)
{
	UOSInt size = this->enc->CountUTF8Chars(dataBuff, buffSize);
	UTF8Char *sbuff = MemAlloc(UTF8Char, size + 1);
	this->enc->UTF8FromBytes(sbuff, dataBuff, buffSize, 0);
	sbuff[size] = 0;
	sb->Append(sbuff);
	MemFree(sbuff);
	return size;
}

UOSInt Text::TextBinEnc::CodePageTextBinEnc::CalcBinSize(const UTF8Char *sbuff)
{
	return this->enc->UTF8CountBytesC(sbuff, Text::StrCharCnt(sbuff));
}

UOSInt Text::TextBinEnc::CodePageTextBinEnc::DecodeBin(const UTF8Char *sbuff, UInt8 *dataBuff)
{
	return this->enc->UTF8ToBytesC(dataBuff, sbuff, Text::StrCharCnt(sbuff));
}

const UTF8Char *Text::TextBinEnc::CodePageTextBinEnc::GetName()
{
	return (const UTF8Char*)"Code Page Encoding";
}
