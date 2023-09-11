#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/TextBinEnc/UTF8LCaseTextBinEnc.h"

Text::TextBinEnc::UTF8LCaseTextBinEnc::UTF8LCaseTextBinEnc()
{
}

Text::TextBinEnc::UTF8LCaseTextBinEnc::~UTF8LCaseTextBinEnc()
{
}

UOSInt Text::TextBinEnc::UTF8LCaseTextBinEnc::EncodeBin(NotNullPtr<Text::StringBuilderUTF8> sb, const UInt8 *dataBuff, UOSInt buffSize)
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

UOSInt Text::TextBinEnc::UTF8LCaseTextBinEnc::CalcBinSize(const UTF8Char *str, UOSInt strLen)
{
	return strLen;
}

UOSInt Text::TextBinEnc::UTF8LCaseTextBinEnc::DecodeBin(const UTF8Char *str, UOSInt strLen, UInt8 *dataBuff)
{
	MemCopyNO(dataBuff, str, strLen);
	return strLen;
}

Text::CStringNN Text::TextBinEnc::UTF8LCaseTextBinEnc::GetName() const
{
	return CSTR("UTF-8 LCase Text");
}
