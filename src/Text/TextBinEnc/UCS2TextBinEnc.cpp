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

UOSInt Text::TextBinEnc::UCS2TextBinEnc::EncodeBin(NotNullPtr<Text::StringBuilderUTF8> sb, const UInt8 *dataBuff, UOSInt buffSize)
{
	if (buffSize & 1)
		return 0;
	NotNullPtr<Text::String> s = Text::String::New((const UTF16Char*)dataBuff, buffSize >> 1);
	sb->Append(s);
	s->Release();
	return buffSize >> 1;
}

UOSInt Text::TextBinEnc::UCS2TextBinEnc::CalcBinSize(const UTF8Char *str, UOSInt strLen)
{
	return Text::StrUTF8_UTF16CntC(str, strLen) << 1;
}

UOSInt Text::TextBinEnc::UCS2TextBinEnc::DecodeBin(const UTF8Char *str, UOSInt strLen, UInt8 *dataBuff)
{
	UOSInt byteCnt = Text::StrUTF8_UTF16CntC(str, strLen) << 1;
	if (byteCnt > 0)
	{
		Text::StrUTF8_UTF16C((UTF16Char*)dataBuff, str, strLen, 0);
	}
	return byteCnt;
}

Text::CStringNN Text::TextBinEnc::UCS2TextBinEnc::GetName() const
{
	return CSTR("UTF16LE Text");
}
