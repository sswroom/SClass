#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/String.h"
#include "Text/StringBuilderUTF8.h"

#define STRINGBUILDER_ALLOCLENG(leng) { \
	UOSInt slen = leng; \
	UOSInt currSize = (UOSInt)(this->buffEnd - this->buff + 1); \
	if (slen + currSize > this->buffSize) \
	{ \
		this->buffSize <<= 1; \
		while (slen + currSize > this->buffSize) \
		{ \
			this->buffSize <<= 1; \
		} \
		UTF8Char *newStr = MemAlloc(UTF8Char, this->buffSize); \
		MemCopyNO(newStr, this->buff, currSize); \
		this->buffEnd = &newStr[currSize - 1]; \
		MemFree(this->buff); \
		this->buff = newStr; \
	} \
}

//#define STRINGBUILDER_ALLOCLENG(leng) this->Text::StringBuilder<UTF8Char>::AllocLeng(leng)

Text::StringBuilderUTF8::StringBuilderUTF8() : Text::StringBuilderUTFText<UTF8Char>()
{
}

Text::StringBuilderUTF8::~StringBuilderUTF8()
{
}

Text::StringBuilderUTF *Text::StringBuilderUTF8::Append(Text::String *s)
{
	if (s == 0)
	{
		return this;
	}
	if (s->leng > 0)
	{
		STRINGBUILDER_ALLOCLENG(s->leng);
		MemCopyNO(this->buffEnd, s->v, s->leng + 1);
		this->buffEnd += s->leng;
	}
	return this;
}

Text::StringBuilderUTF *Text::StringBuilderUTF8::Append(const UTF8Char *s)
{
	if (s == 0)
	{
		return this;
	}
	UOSInt len = Text::StrCharCnt(s);
	if (len > 0)
	{
		STRINGBUILDER_ALLOCLENG(len);
		this->buffEnd = Text::StrConcatC(this->buffEnd, s, len);
	}
	return this;
}

Text::StringBuilderUTF *Text::StringBuilderUTF8::AppendC(const UTF8Char *s, UOSInt charCnt)
{
	if (charCnt > 0)
	{
		STRINGBUILDER_ALLOCLENG(charCnt);
		MemCopyNO(this->buffEnd, s, charCnt);
		this->buffEnd += charCnt;
		this->buffEnd[0] = 0;
	}
	return this;
}

Text::StringBuilderUTF *Text::StringBuilderUTF8::AppendS(const UTF8Char *s, UOSInt maxLen)
{
	if (maxLen > 0)
	{
		STRINGBUILDER_ALLOCLENG(maxLen);
		this->buffEnd = Text::StrConcatS(this->buffEnd, s, maxLen);
	}
	return this;
}

Text::StringBuilderUTF *Text::StringBuilderUTF8::AppendChar(UTF32Char c, UOSInt repCnt)
{
	UTF8Char oc[6];
	if (c < 0x80)
	{
		STRINGBUILDER_ALLOCLENG(repCnt);
		oc[0] = (UInt8)c;
		while (repCnt-- > 0)
		{
			this->buffEnd[0] = oc[0];
			this->buffEnd += 1;
		}
	}
	else if (c < 0x800)
	{
		STRINGBUILDER_ALLOCLENG(2 * repCnt);
		oc[0] = (UTF8Char)(0xc0 | (c >> 6));
		oc[1] = (UTF8Char)(0x80 | (c & 0x3f));
		while (repCnt-- > 0)
		{
			this->buffEnd[0] = oc[0];
			this->buffEnd[1] = oc[1];
			this->buffEnd += 2;
		}
	}
	else if (c < 0x10000)
	{
		STRINGBUILDER_ALLOCLENG(3 * repCnt);
		oc[0] = (UTF8Char)(0xe0 | (c >> 12));
		oc[1] = (UTF8Char)(0x80 | ((c >> 6) & 0x3f));
		oc[2] = (UTF8Char)(0x80 | (c & 0x3f));
		while (repCnt-- > 0)
		{
			this->buffEnd[0] = oc[0];
			this->buffEnd[1] = oc[1];
			this->buffEnd[2] = oc[2];
			this->buffEnd += 3;
		}
	}
	else if (c < 0x200000)
	{
		STRINGBUILDER_ALLOCLENG(4 * repCnt);
		oc[0] = (UTF8Char)(0xf0 | (c >> 18));
		oc[1] = (UTF8Char)(0x80 | ((c >> 12) & 0x3f));
		oc[2] = (UTF8Char)(0x80 | ((c >> 6) & 0x3f));
		oc[3] = (UTF8Char)(0x80 | (c & 0x3f));
		while (repCnt-- > 0)
		{
			this->buffEnd[0] = oc[0];
			this->buffEnd[1] = oc[1];
			this->buffEnd[2] = oc[2];
			this->buffEnd[3] = oc[3];
			this->buffEnd += 4;
		}
	}
	else if (c < 0x4000000)
	{
		STRINGBUILDER_ALLOCLENG(5 * repCnt);
		oc[0] = (UTF8Char)(0xf8 | (c >> 24));
		oc[1] = (UTF8Char)(0x80 | ((c >> 18) & 0x3f));
		oc[2] = (UTF8Char)(0x80 | ((c >> 12) & 0x3f));
		oc[3] = (UTF8Char)(0x80 | ((c >> 6) & 0x3f));
		oc[4] = (UTF8Char)(0x80 | (c & 0x3f));
		while (repCnt-- > 0)
		{
			this->buffEnd[0] = oc[0];
			this->buffEnd[1] = oc[1];
			this->buffEnd[2] = oc[2];
			this->buffEnd[3] = oc[3];
			this->buffEnd[4] = oc[4];
			this->buffEnd += 5;
		}
	}
	else
	{
		STRINGBUILDER_ALLOCLENG(6 * repCnt);
		oc[0] = (UTF8Char)(0xfc | (c >> 30));
		oc[1] = (UTF8Char)(0x80 | ((c >> 24) & 0x3f));
		oc[2] = (UTF8Char)(0x80 | ((c >> 18) & 0x3f));
		oc[3] = (UTF8Char)(0x80 | ((c >> 12) & 0x3f));
		oc[4] = (UTF8Char)(0x80 | ((c >> 6) & 0x3f));
		oc[5] = (UTF8Char)(0x80 | (c & 0x3f));
		while (repCnt-- > 0)
		{
			this->buffEnd[0] = oc[0];
			this->buffEnd[1] = oc[1];
			this->buffEnd[2] = oc[2];
			this->buffEnd[3] = oc[3];
			this->buffEnd[4] = oc[4];
			this->buffEnd[5] = oc[5];
			this->buffEnd += 6;
		}
	}
	this->buffEnd[0] = 0;
	return this;
}

Text::StringBuilderUTF8 *Text::StringBuilderUTF8::AppendCSV(const UTF8Char **sarr, UOSInt nStr)
{
	Text::String *s;
	UOSInt i;
	i = 0;
	while (i < nStr)
	{
		s = Text::String::NewCSVRec(sarr[i]);
		if (i > 0)
			this->AppendChar(',', 1);
		this->AppendC(s->v, s->leng);
		s->Release();
		i++;
	}
	return this;
}

Text::StringBuilderUTF8 *Text::StringBuilderUTF8::AppendToUpper(const UTF8Char *s)
{
	UOSInt len = Text::StrCharCnt(s);
	STRINGBUILDER_ALLOCLENG(len);
	this->buffEnd = Text::StrToUpper(this->buffEnd, s);
	return this;
}

Text::StringBuilderUTF8 *Text::StringBuilderUTF8::AppendToLower(const UTF8Char *s)
{
	UOSInt len = Text::StrCharCnt(s);
	STRINGBUILDER_ALLOCLENG(len);
	this->buffEnd = Text::StrToLower(this->buffEnd, s);
	return this;
}

Text::StringBuilderUTF8 *Text::StringBuilderUTF8::RemoveANSIEscapes()
{
	this->buffEnd = Text::StrRemoveANSIEscapes(this->buff);
	return this;
}

Bool Text::StringBuilderUTF8::EqualsC(const UTF8Char *s, UOSInt len)
{
	return Text::StrEqualsC(this->buff, (UOSInt)(this->buffEnd - this->buff), s, len);
}

Bool Text::StringBuilderUTF8::EqualsICaseC(const UTF8Char *s, UOSInt len)
{
	if ((UOSInt)(this->buffEnd - this->buff) != len)
	{
		return false;
	}
	return Text::StrEqualsICase(this->buff, s);
}

Bool Text::StringBuilderUTF8::StartsWithC(const UTF8Char *s, UOSInt len)
{
	return Text::StrStartsWithC(this->buff, (UOSInt)(this->buffEnd - this->buff), s, len);
}

UOSInt Text::StringBuilderUTF8::IndexOfC(const UTF8Char *s, UOSInt len)
{
	return Text::StrIndexOfC(this->buff, (UOSInt)(this->buffEnd - this->buff), s, len);
}

void Text::StringBuilderUTF8::TrimC()
{
	this->buffEnd = Text::StrTrimC(this->buff, (UOSInt)(this->buffEnd - this->buff));
}

void Text::StringBuilderUTF8::TrimC(UOSInt index)
{
	this->buffEnd = Text::StrTrimC(&this->buff[index], (UOSInt)(this->buffEnd - this->buff) - index);
}
