#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/TextBinEnc/CodePageTextBinEnc.h"

Text::TextBinEnc::CodePageTextBinEnc::CodePageTextBinEnc(UInt32 codePage)
{
	NEW_CLASSNN(this->enc, Text::Encoding(codePage));
}

Text::TextBinEnc::CodePageTextBinEnc::~CodePageTextBinEnc()
{
	this->enc.Delete();
}

UIntOS Text::TextBinEnc::CodePageTextBinEnc::EncodeBin(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UInt8> dataBuff, UIntOS buffSize) const
{
	UIntOS size = this->enc->CountUTF8Chars(dataBuff, buffSize);
	UnsafeArray<UTF8Char> sbuff = MemAllocArr(UTF8Char, size + 1);
	this->enc->UTF8FromBytes(sbuff, dataBuff, buffSize, 0);
	sbuff[size] = 0;
	sb->AppendC(sbuff, size);
	MemFreeArr(sbuff);
	return size;
}

UIntOS Text::TextBinEnc::CodePageTextBinEnc::CalcBinSize(Text::CStringNN str) const
{
	return this->enc->UTF8CountBytesC(str.v, str.leng);
}

UIntOS Text::TextBinEnc::CodePageTextBinEnc::DecodeBin(Text::CStringNN str, UnsafeArray<UInt8> dataBuff) const
{
	return this->enc->UTF8ToBytesC(dataBuff, str.v, str.leng);
}

Text::CStringNN Text::TextBinEnc::CodePageTextBinEnc::GetName() const
{
	return CSTR("Code Page Encoding");
}
