#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/TextBinEnc/UTF8UCaseTextBinEnc.h"

Text::TextBinEnc::UTF8UCaseTextBinEnc::UTF8UCaseTextBinEnc()
{
}

Text::TextBinEnc::UTF8UCaseTextBinEnc::~UTF8UCaseTextBinEnc()
{
}

UOSInt Text::TextBinEnc::UTF8UCaseTextBinEnc::EncodeBin(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UInt8> dataBuff, UOSInt buffSize) const
{
	UTF8Char c;
	UOSInt i = buffSize;
	while (i-- > 0)
	{
		c = *dataBuff++;
		if (c >= 'a' && c <= 'z')
		{
			sb->AppendUTF8Char((UTF8Char)(c - 32));
		}
		else
		{
			sb->AppendUTF8Char(c);
		}
	}
	return buffSize;
}

UOSInt Text::TextBinEnc::UTF8UCaseTextBinEnc::CalcBinSize(Text::CStringNN str) const
{
	return str.leng;
}

UOSInt Text::TextBinEnc::UTF8UCaseTextBinEnc::DecodeBin(Text::CStringNN str, UnsafeArray<UInt8> dataBuff) const
{
	MemCopyNO(dataBuff.Ptr(), str.v.Ptr(), str.leng);
	return str.leng;
}

Text::CStringNN Text::TextBinEnc::UTF8UCaseTextBinEnc::GetName() const
{
	return CSTR("UTF-8 UCase Text");
}
