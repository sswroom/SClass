#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/TextBinEnc/UCS2TextBinEnc.h"

Text::TextBinEnc::UCS2TextBinEnc::UCS2TextBinEnc()
{
}

Text::TextBinEnc::UCS2TextBinEnc::~UCS2TextBinEnc()
{
}

UOSInt Text::TextBinEnc::UCS2TextBinEnc::EncodeBin(Text::StringBuilderUTF *sb, const UInt8 *dataBuff, UOSInt buffSize)
{
	if (buffSize & 1)
		return 0;
	const UTF16Char *wptr = Text::StrCopyNewC((const UTF16Char*)dataBuff, buffSize >> 1);
	const UTF8Char *csptr = Text::StrToUTF8New(wptr);
	sb->Append(csptr);
	Text::StrDelNew(csptr);
	Text::StrDelNew(wptr);
	return buffSize >> 1;
}

UOSInt Text::TextBinEnc::UCS2TextBinEnc::CalcBinSize(const UTF8Char *sbuff)
{
	return Text::StrUTF8_UTF16Cnt(sbuff, -1) << 1;
}

UOSInt Text::TextBinEnc::UCS2TextBinEnc::DecodeBin(const UTF8Char *sbuff, UInt8 *dataBuff)
{
	UOSInt byteCnt = Text::StrUTF8_UTF16Cnt(sbuff, -1) << 1;
	if (byteCnt > 0)
	{
		Text::StrUTF8_UTF16((UTF16Char*)dataBuff, sbuff, -1, 0);
	}
	return byteCnt;
}

const UTF8Char *Text::TextBinEnc::UCS2TextBinEnc::GetName()
{
	return (const UTF8Char*)"Unicode Text";
}
