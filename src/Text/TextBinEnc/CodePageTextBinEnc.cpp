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

UOSInt Text::TextBinEnc::CodePageTextBinEnc::EncodeBin(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UInt8> dataBuff, UOSInt buffSize) const
{
	UOSInt size = this->enc->CountUTF8Chars(dataBuff, buffSize);
	UTF8Char *sbuff = MemAlloc(UTF8Char, size + 1);
	this->enc->UTF8FromBytes(sbuff, dataBuff, buffSize, 0);
	sbuff[size] = 0;
	sb->AppendC(sbuff, size);
	MemFree(sbuff);
	return size;
}

UOSInt Text::TextBinEnc::CodePageTextBinEnc::CalcBinSize(Text::CStringNN str) const
{
	return this->enc->UTF8CountBytesC(str.v, str.leng);
}

UOSInt Text::TextBinEnc::CodePageTextBinEnc::DecodeBin(Text::CStringNN str, UnsafeArray<UInt8> dataBuff) const
{
	return this->enc->UTF8ToBytesC(dataBuff, str.v, str.leng);
}

Text::CStringNN Text::TextBinEnc::CodePageTextBinEnc::GetName() const
{
	return CSTR("Code Page Encoding");
}
