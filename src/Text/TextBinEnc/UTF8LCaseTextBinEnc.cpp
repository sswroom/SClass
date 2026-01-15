#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/TextBinEnc/UTF8LCaseTextBinEnc.h"

Text::TextBinEnc::UTF8LCaseTextBinEnc::UTF8LCaseTextBinEnc()
{
}

Text::TextBinEnc::UTF8LCaseTextBinEnc::~UTF8LCaseTextBinEnc()
{
}

UIntOS Text::TextBinEnc::UTF8LCaseTextBinEnc::EncodeBin(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UInt8> dataBuff, UIntOS buffSize) const
{
	UTF8Char c;
	UIntOS i = buffSize;
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

UIntOS Text::TextBinEnc::UTF8LCaseTextBinEnc::CalcBinSize(Text::CStringNN str) const
{
	return str.leng;
}

UIntOS Text::TextBinEnc::UTF8LCaseTextBinEnc::DecodeBin(Text::CStringNN str, UnsafeArray<UInt8> dataBuff) const
{
	MemCopyNO(dataBuff.Ptr(), str.v.Ptr(), str.leng);
	return str.leng;
}

Text::CStringNN Text::TextBinEnc::UTF8LCaseTextBinEnc::GetName() const
{
	return CSTR("UTF-8 LCase Text");
}
