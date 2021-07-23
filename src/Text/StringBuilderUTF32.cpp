#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilderUTF32.h"

Text::StringBuilderUTF32::StringBuilderUTF32() : Text::StringBuilderUTFText<UTF32Char>()
{
}

Text::StringBuilderUTF32::~StringBuilderUTF32()
{
}

Text::StringBuilderUTF *Text::StringBuilderUTF32::Append(const UTF8Char *s)
{
	OSInt slen = Text::StrUTF8_UTF32Cnt(s);
	if (slen > 0)
	{
		this->AllocLeng(slen);
		this->buffEnd = Text::StrUTF8_UTF32(this->buffEnd, s, 0);
	}
	return this;
}

Text::StringBuilderUTF *Text::StringBuilderUTF32::AppendC(const UTF8Char *s, UOSInt charCnt)
{
	OSInt slen = Text::StrUTF8_UTF32CntC(s, charCnt);
	if (slen > 0)
	{
		this->AllocLeng(slen);
		this->buffEnd = Text::StrUTF8_UTF32C(this->buffEnd, s, charCnt, 0);
	}
	return this;
}

Text::StringBuilderUTF *Text::StringBuilderUTF32::AppendS(const UTF8Char *s, UOSInt maxLen)
{
	OSInt len = Text::StrCharCntS(s, maxLen);
	OSInt slen = Text::StrUTF8_UTF32CntC(s, len);
	if (slen > 0)
	{
		this->AllocLeng(slen);
		this->buffEnd = Text::StrUTF8_UTF32C(this->buffEnd, s, len, 0);
	}
	return this;
}

Text::StringBuilderUTF *Text::StringBuilderUTF32::AppendChar(UTF32Char c, UOSInt repCnt)
{
	this->AllocLeng(repCnt);
	while (repCnt-- > 0)
	{
		*this->buffEnd++ = c;
	}
	this->buffEnd[0] = 0;
	return this;
}

Text::StringBuilderUTF32 *Text::StringBuilderUTF32::AppendCSV(const UTF32Char **sarr, UOSInt nStr)
{
	const UTF32Char *csptr;
	UOSInt i;
	i = 0;
	while (i < nStr)
	{
		csptr = StrToNewCSVRec(sarr[i]);
		if (i > 0)
		{
			this->AllocLeng(1);
			*(this->buffEnd)++ = ',';
			this->buffEnd[0] = 0;
		}
		
		OSInt slen = Text::StrCharCnt(csptr);
		if (slen > 0)
		{
			this->AllocLeng(slen);
			this->buffEnd = Text::StrConcat(this->buffEnd, csptr);
		}
		Text::StrDelNew(csptr);
		i++;
	}
	return this;
}

Text::StringBuilderUTF32 *Text::StringBuilderUTF32::AppendToUpper(const UTF32Char *s)
{
	OSInt slen = Text::StrCharCnt(s);
	this->AllocLeng(slen);
	this->buffEnd = Text::StrToUpper(this->buffEnd, s);
	return this;
}

Text::StringBuilderUTF32 *Text::StringBuilderUTF32::AppendToLower(const UTF32Char *s)
{
	OSInt slen = Text::StrCharCnt(s);
	this->AllocLeng(slen);
	this->buffEnd = Text::StrToLower(this->buffEnd, s);
	return this;
}

