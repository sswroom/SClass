#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/String.h"
#include "Text/StringBuilderUTF8.h"

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
		this->Text::StringBuilder<UTF8Char>::AllocLeng(s->leng);
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
	UOSInt slen = Text::StrCharCnt(s);
	if (slen > 0)
	{
		this->Text::StringBuilder<UTF8Char>::AllocLeng(slen);
		this->buffEnd = Text::StrConcatC(this->buffEnd, s, slen);
	}
	return this;
}

Text::StringBuilderUTF *Text::StringBuilderUTF8::AppendC(const UTF8Char *s, UOSInt charCnt)
{
	if (charCnt > 0)
	{
		this->Text::StringBuilder<UTF8Char>::AllocLeng(charCnt);
		MemCopyNO(this->buffEnd, s, charCnt + 1);
		this->buffEnd += charCnt;
	}
	return this;
}

Text::StringBuilderUTF *Text::StringBuilderUTF8::AppendS(const UTF8Char *s, UOSInt maxLen)
{
	if (maxLen > 0)
	{
		this->Text::StringBuilder<UTF8Char>::AllocLeng(maxLen);
		this->buffEnd = Text::StrConcatS(this->buffEnd, s, maxLen);
	}
	return this;
}

Text::StringBuilderUTF *Text::StringBuilderUTF8::AppendChar(UTF32Char c, UOSInt repCnt)
{
	UTF8Char oc[6];
	if (c < 0x80)
	{
		this->Text::StringBuilder<UTF8Char>::AllocLeng(repCnt);
		oc[0] = (UInt8)c;
		while (repCnt-- > 0)
		{
			this->buffEnd[0] = oc[0];
			this->buffEnd += 1;
		}
	}
	else if (c < 0x800)
	{
		this->Text::StringBuilder<UTF8Char>::AllocLeng(2 * repCnt);
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
		this->Text::StringBuilder<UTF8Char>::AllocLeng(3 * repCnt);
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
		this->Text::StringBuilder<UTF8Char>::AllocLeng(4 * repCnt);
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
		this->Text::StringBuilder<UTF8Char>::AllocLeng(5 * repCnt);
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
		this->Text::StringBuilder<UTF8Char>::AllocLeng(6 * repCnt);
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
	UOSInt slen = Text::StrCharCnt(s);
	this->Text::StringBuilder<UTF8Char>::AllocLeng(slen);
	this->buffEnd = Text::StrToUpper(this->buffEnd, s);
	return this;
}

Text::StringBuilderUTF8 *Text::StringBuilderUTF8::AppendToLower(const UTF8Char *s)
{
	UOSInt slen = Text::StrCharCnt(s);
	this->Text::StringBuilder<UTF8Char>::AllocLeng(slen);
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

Bool Text::StringBuilderUTF8::StartsWithC(const UTF8Char *s, UOSInt len)
{
	return Text::StrStartsWithC(this->buff, (UOSInt)(this->buffEnd - this->buff), s, len);
}

UOSInt Text::StringBuilderUTF8::IndexOfC(const UTF8Char *s, UOSInt len)
{
	return Text::StrIndexOfC(this->buff, (UOSInt)(this->buffEnd - this->buff), s, len);
}
