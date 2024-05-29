#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/TextBinEnc/UTF8LCaseTextBinEnc.h"

Text::TextBinEnc::UTF8LCaseTextBinEnc::UTF8LCaseTextBinEnc()
{
}

Text::TextBinEnc::UTF8LCaseTextBinEnc::~UTF8LCaseTextBinEnc()
{
}

UOSInt Text::TextBinEnc::UTF8LCaseTextBinEnc::EncodeBin(NN<Text::StringBuilderUTF8> sb, const UInt8 *dataBuff, UOSInt buffSize) const
{
	UTF8Char c;
	UOSInt i = buffSize;
	while (i-- > 0)
	{
		c = *dataBuff++;
		if (c >= 'A' && c <= 'Z')
		{
			sb->AppendUTF8Char((UTF8Char)(c + 32));
		}
		else
		{
			sb->AppendUTF8Char(c);
		}
	}
	return buffSize;
}

UOSInt Text::TextBinEnc::UTF8LCaseTextBinEnc::CalcBinSize(Text::CStringNN str) const
{
	return str.leng;
}

UOSInt Text::TextBinEnc::UTF8LCaseTextBinEnc::DecodeBin(Text::CStringNN str, UInt8 *dataBuff) const
{
	MemCopyNO(dataBuff, str.v, str.leng);
	return str.leng;
}

Text::CStringNN Text::TextBinEnc::UTF8LCaseTextBinEnc::GetName() const
{
	return CSTR("UTF-8 LCase Text");
}
