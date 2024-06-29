#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/String.h"
#include "Text/TextBinEnc/UCS2TextBinEnc.h"

Text::TextBinEnc::UCS2TextBinEnc::UCS2TextBinEnc()
{
}

Text::TextBinEnc::UCS2TextBinEnc::~UCS2TextBinEnc()
{
}

UOSInt Text::TextBinEnc::UCS2TextBinEnc::EncodeBin(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UInt8> dataBuff, UOSInt buffSize) const
{
	if (buffSize & 1)
		return 0;
	NN<Text::String> s = Text::String::NewW((const UTF16Char*)dataBuff.Ptr(), buffSize >> 1);
	sb->Append(s);
	s->Release();
	return buffSize >> 1;
}

UOSInt Text::TextBinEnc::UCS2TextBinEnc::CalcBinSize(Text::CStringNN str) const
{
	return Text::StrUTF8_UTF16CntC(str.v, str.leng) << 1;
}

UOSInt Text::TextBinEnc::UCS2TextBinEnc::DecodeBin(Text::CStringNN str, UnsafeArray<UInt8> dataBuff) const
{
	UOSInt byteCnt = Text::StrUTF8_UTF16CntC(str.v, str.leng) << 1;
	if (byteCnt > 0)
	{
		Text::StrUTF8_UTF16C((UTF16Char*)dataBuff.Ptr(), str.v, str.leng, 0);
	}
	return byteCnt;
}

Text::CStringNN Text::TextBinEnc::UCS2TextBinEnc::GetName() const
{
	return CSTR("UTF16LE Text");
}
