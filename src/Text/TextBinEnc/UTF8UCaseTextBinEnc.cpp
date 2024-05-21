#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/TextBinEnc/UTF8UCaseTextBinEnc.h"

Text::TextBinEnc::UTF8UCaseTextBinEnc::UTF8UCaseTextBinEnc()
{
}

Text::TextBinEnc::UTF8UCaseTextBinEnc::~UTF8UCaseTextBinEnc()
{
}

UOSInt Text::TextBinEnc::UTF8UCaseTextBinEnc::EncodeBin(NN<Text::StringBuilderUTF8> sb, const UInt8 *dataBuff, UOSInt buffSize) const
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

UOSInt Text::TextBinEnc::UTF8UCaseTextBinEnc::CalcBinSize(const UTF8Char *str, UOSInt strLen) const
{
	return strLen;
}

UOSInt Text::TextBinEnc::UTF8UCaseTextBinEnc::DecodeBin(const UTF8Char *str, UOSInt strLen, UInt8 *dataBuff) const
{
	MemCopyNO(dataBuff, str, strLen);
	return strLen;
}

Text::CStringNN Text::TextBinEnc::UTF8UCaseTextBinEnc::GetName() const
{
	return CSTR("UTF-8 UCase Text");
}
