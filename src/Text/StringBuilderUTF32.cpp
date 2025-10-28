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

NN<Text::StringBuilderUTF> Text::StringBuilderUTF32::Append(NN<Text::PString> s)
{
	UOSInt slen = Text::StrUTF8_UTF32CntC(s->v, s->leng);
	if (slen > 0)
	{
		this->AllocLeng(slen);
		this->buffEnd = Text::StrUTF8_UTF32C(this->buffEnd.Ptr(), s->v, s->leng, 0);
	}
	return *this;
}

NN<Text::StringBuilderUTF> Text::StringBuilderUTF32::Append(UnsafeArray<const UTF8Char> s)
{
	UOSInt slen = Text::StrUTF8_UTF32Cnt(s);
	if (slen > 0)
	{
		this->AllocLeng(slen);
		this->buffEnd = Text::StrUTF8_UTF32(this->buffEnd.Ptr(), s, 0);
	}
	return *this;
}

NN<Text::StringBuilderUTF> Text::StringBuilderUTF32::AppendC(UnsafeArray<const UTF8Char> s, UOSInt charCnt)
{
	UOSInt slen = Text::StrUTF8_UTF32CntC(s, charCnt);
	if (slen > 0)
	{
		this->AllocLeng(slen);
		this->buffEnd = Text::StrUTF8_UTF32C(this->buffEnd.Ptr(), s, charCnt, 0);
	}
	return *this;
}

NN<Text::StringBuilderUTF> Text::StringBuilderUTF32::AppendS(UnsafeArray<const UTF8Char> s, UOSInt maxLen)
{
	UOSInt len = Text::StrCharCntS(s, maxLen);
	UOSInt slen = Text::StrUTF8_UTF32CntC(s, len);
	if (slen > 0)
	{
		this->AllocLeng(slen);
		this->buffEnd = Text::StrUTF8_UTF32C(this->buffEnd.Ptr(), s, len, 0);
	}
	return *this;
}

NN<Text::StringBuilderUTF> Text::StringBuilderUTF32::AppendChar(UTF32Char c, UOSInt repCnt)
{
	this->AllocLeng(repCnt);
	while (repCnt-- > 0)
	{
		*this->buffEnd++ = c;
	}
	this->buffEnd[0] = 0;
	return *this;
}

NN<Text::StringBuilderUTF32> Text::StringBuilderUTF32::AppendCSV(UnsafeArray<UnsafeArray<const UTF32Char>> sarr, UOSInt nStr)
{
	UnsafeArray<const UTF32Char> csptr;
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
		
		UOSInt slen = Text::StrCharCnt(csptr);
		if (slen > 0)
		{
			this->AllocLeng(slen);
			this->buffEnd = Text::StrConcat(this->buffEnd, csptr);
		}
		Text::StrDelNew(csptr);
		i++;
	}
	return *this;
}

NN<Text::StringBuilderUTF32> Text::StringBuilderUTF32::AppendToUpper(UnsafeArray<const UTF32Char> s)
{
	UOSInt slen = Text::StrCharCnt(s);
	this->AllocLeng(slen);
	this->buffEnd = Text::StrToUpper(this->buffEnd, s);
	return *this;
}

NN<Text::StringBuilderUTF32> Text::StringBuilderUTF32::AppendToLower(UnsafeArray<const UTF32Char> s)
{
	UOSInt slen = Text::StrCharCnt(s);
	this->AllocLeng(slen);
	this->buffEnd = Text::StrToLower(this->buffEnd, s);
	return *this;
}

