#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/String.h"
#include "Text/TextBinEnc/UTF16BETextBinEnc.h"

Text::TextBinEnc::UTF16BETextBinEnc::UTF16BETextBinEnc()
{
}

Text::TextBinEnc::UTF16BETextBinEnc::~UTF16BETextBinEnc()
{
}

UOSInt Text::TextBinEnc::UTF16BETextBinEnc::EncodeBin(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UInt8> dataBuff, UOSInt buffSize) const
{
	buffSize = buffSize & (UOSInt)~1;
	UOSInt initSize = sb->leng;
	UOSInt i = 0;
	UInt16 c;
	UInt16 c2;
	while (i < buffSize)
	{
		c = ReadMUInt16(&dataBuff[i]);
		if (i + 2 < buffSize && c >= 0xd800 && c < 0xdc00 && (c2 = ReadMUInt16(&dataBuff[i + 2])) >= 0xdc00 && c2 < 0xe000)
		{
			sb->AppendChar((UTF32Char)(0x10000 + ((c - 0xd800) << 10) + (c2 - 0xdc00)), 1);
			i += 4;
		}
		else
		{
			sb->AppendChar(c, 1);
			i += 2;
		}
	}
	return sb->leng - initSize;
}

UOSInt Text::TextBinEnc::UTF16BETextBinEnc::CalcBinSize(Text::CStringNN str) const
{
	return Text::StrUTF8_UTF16CntC(str.v, str.leng) << 1;
}

UOSInt Text::TextBinEnc::UTF16BETextBinEnc::DecodeBin(Text::CStringNN s, UnsafeArray<UInt8> dataBuff) const
{
	UnsafeArray<UInt8> currBuff = dataBuff;
	UnsafeArray<const UTF8Char> str = s.v;
	UnsafeArray<const UTF8Char> strEnd = s.GetEndPtr();
	UTF32Char c;
	while (str < strEnd)
	{
		str = Text::StrReadChar(str, c);
		if (c >= 0x10000)
		{
			WriteMUInt16(&currBuff[0], (UTF16Char)(0xd800 + (c >> 10)));
			WriteMUInt16(&currBuff[2], (UTF16Char)((c & 0x3ff) + 0xdc00));
			currBuff += 4;
		}
		else
		{
			WriteMUInt16(&currBuff[0], (UTF16Char)c);
			currBuff += 2;
		}
	}
	return (UOSInt)(currBuff - dataBuff);
}

Text::CStringNN Text::TextBinEnc::UTF16BETextBinEnc::GetName() const
{
	return CSTR("UTF16BE Text");
}
