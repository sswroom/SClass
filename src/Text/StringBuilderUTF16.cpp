#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilderUTF16.h"

Text::StringBuilderUTF16::StringBuilderUTF16() : Text::StringBuilderUTFText<UTF16Char>()
{
}

Text::StringBuilderUTF16::~StringBuilderUTF16()
{
}

Text::StringBuilderUTF *Text::StringBuilderUTF16::Append(const UTF8Char *s)
{
	UOSInt slen = Text::StrUTF8_UTF16Cnt(s);
	if (slen > 0)
	{
		this->AllocLeng(slen);
		this->buffEnd = Text::StrUTF8_UTF16(this->buffEnd, s, 0);
	}
	return this;
}

Text::StringBuilderUTF *Text::StringBuilderUTF16::AppendC(const UTF8Char *s, UOSInt charCnt)
{
	UOSInt slen = Text::StrUTF8_UTF16CntC(s, charCnt);
	if (slen > 0)
	{
		this->AllocLeng(slen);
		this->buffEnd = Text::StrUTF8_UTF16C(this->buffEnd, s, charCnt, 0);
	}
	return this;
}

Text::StringBuilderUTF *Text::StringBuilderUTF16::AppendS(const UTF8Char *s, UOSInt maxLen)
{
	UOSInt len = Text::StrCharCntS(s, maxLen);
	UOSInt slen = Text::StrUTF8_UTF16CntC(s, len);
	if (slen > 0)
	{
		this->AllocLeng(slen);
		this->buffEnd = Text::StrUTF8_UTF16C(this->buffEnd, s, len, 0);
	}
	return this;
}

Text::StringBuilderUTF *Text::StringBuilderUTF16::AppendChar(UTF32Char c, UOSInt repCnt)
{
	if (c >= 0x10000)
	{
		this->AllocLeng(repCnt * 2);
		UTF16Char oc[2];
		oc[0] = (0xd800 + (c >> 10));
		oc[1] = (c & 0x3ff) + 0xdc00;
		while (repCnt-- > 0)
		{
			this->buffEnd[0] = oc[0];
			this->buffEnd[1] = oc[1];
			this->buffEnd += 2;
		}
		this->buffEnd[0] = 0;
	}
	else
	{
		this->AllocLeng(repCnt);
		UTF16Char oc = (UTF16Char)c;
		while (repCnt-- > 0)
		{
			*this->buffEnd++ = oc;
		}
		this->buffEnd[0] = 0;
	}
	return this;
}

Text::StringBuilderUTF16 *Text::StringBuilderUTF16::AppendCSV(const UTF16Char **sarr, UOSInt nStr)
{
	const UTF16Char *csptr;
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

Text::StringBuilderUTF16 *Text::StringBuilderUTF16::AppendToUpper(const UTF16Char *s)
{
	OSInt slen = Text::StrCharCnt(s);
	this->AllocLeng(slen);
	this->buffEnd = Text::StrToUpper(this->buffEnd, s);
	return this;
}

Text::StringBuilderUTF16 *Text::StringBuilderUTF16::AppendToLower(const UTF16Char *s)
{
	OSInt slen = Text::StrCharCnt(s);
	this->AllocLeng(slen);
	this->buffEnd = Text::StrToLower(this->buffEnd, s);
	return this;
}

Text::StringBuilderUTF *Text::StringBuilderUTF::Append(const UTF8Char *s)
{
	return this;
}

Text::StringBuilderUTF *Text::StringBuilderUTF::AppendC(const UTF8Char *s, UOSInt cnt)
{
	return this;
}

