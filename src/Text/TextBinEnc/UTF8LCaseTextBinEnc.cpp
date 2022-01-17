#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/TextBinEnc/UTF8LCaseTextBinEnc.h"

Text::TextBinEnc::UTF8LCaseTextBinEnc::UTF8LCaseTextBinEnc()
{
}

Text::TextBinEnc::UTF8LCaseTextBinEnc::~UTF8LCaseTextBinEnc()
{
}

UOSInt Text::TextBinEnc::UTF8LCaseTextBinEnc::EncodeBin(Text::StringBuilderUTF *sb, const UInt8 *dataBuff, UOSInt buffSize)
{
	UTF8Char c;
	UOSInt i = buffSize;
	while (i-- > 0)
	{
		c = *dataBuff++;
		if (c >= 'A' && c <= 'Z')
		{
			sb->AppendChar((UTF32Char)(c + 32), 1);
		}
		else
		{
			sb->AppendChar(c, 1);
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

Text::CString Text::TextBinEnc::UTF8LCaseTextBinEnc::GetName()
{
	return {UTF8STRC("UTF-8 LCase Text")};
}
