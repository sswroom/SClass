#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/String.h"
#include "Text/StringBuilderUTF8.h"

#define STRINGBUILDER_ALLOCLENG(leng) this->AllocLeng(leng)

Text::StringBuilderUTF8 *Text::StringBuilderUTF8::Append(Text::StringBase<UTF8Char> *s)
{
	if (s == 0)
	{
		return this;
	}
	if (s->leng > 0)
	{
		STRINGBUILDER_ALLOCLENG(s->leng);
		MemCopyNO(&this->v[this->leng], s->v, s->leng + 1);
		this->leng += s->leng;
	}
	return this;
}

Text::StringBuilderUTF8 *Text::StringBuilderUTF8::Append(NotNullPtr<Text::String> s)
{
//	if (s->leng > 0)
//	{
		STRINGBUILDER_ALLOCLENG(s->leng);
		MemCopyNO(&this->v[this->leng], s->v, s->leng + 1);
		this->leng += s->leng;
//	}
	return this;
}

Text::StringBuilderUTF8 *Text::StringBuilderUTF8::Append(Text::StringBase<const UTF8Char> *s)
{
	if (s == 0)
	{
		return this;
	}
	if (s->leng > 0)
	{
		STRINGBUILDER_ALLOCLENG(s->leng);
		MemCopyNO(&this->v[this->leng], s->v, s->leng + 1);
		this->leng += s->leng;
	}
	return this;
}

Text::StringBuilderUTF8 *Text::StringBuilderUTF8::Append(const Text::StringBase<UTF8Char> &s)
{
	if (s.leng > 0)
	{
		STRINGBUILDER_ALLOCLENG(s.leng);
		MemCopyNO(&this->v[this->leng], s.v, s.leng + 1);
		this->leng += s.leng;
	}
	return this;
}

Text::StringBuilderUTF8 *Text::StringBuilderUTF8::Append(const Text::StringBase<const UTF8Char> &s)
{
	if (s.leng > 0)
	{
		STRINGBUILDER_ALLOCLENG(s.leng);
		MemCopyNO(&this->v[this->leng], s.v, s.leng + 1);
		this->leng += s.leng;
	}
	return this;
}

Text::StringBuilderUTF8 *Text::StringBuilderUTF8::AppendW(const WChar *s)
{
	UOSInt charCnt = Text::StrWChar_UTF8Cnt(s);
	STRINGBUILDER_ALLOCLENG(charCnt);
	Text::StrWChar_UTF8(&this->v[this->leng], s);
	this->leng += charCnt;
	return this;
}

Text::StringBuilderUTF8 *Text::StringBuilderUTF8::AppendW(const WChar *s, UOSInt len)
{
	UOSInt charCnt = Text::StrWChar_UTF8CntC(s, len);
	STRINGBUILDER_ALLOCLENG(charCnt);
	Text::StrWChar_UTF8C(&this->v[this->leng], s, len);
	this->leng += charCnt;
	this->v[this->leng] = 0;
	return this;
}

Text::StringBuilderUTF8 *Text::StringBuilderUTF8::AppendUTF16(const UTF16Char *s)
{
	UOSInt charCnt = Text::StrUTF16_UTF8Cnt(s);
	STRINGBUILDER_ALLOCLENG(charCnt);
	Text::StrUTF16_UTF8(&this->v[this->leng], s);
	this->leng += charCnt;
	return this;
}

Text::StringBuilderUTF8 *Text::StringBuilderUTF8::AppendUTF16(const UTF16Char *s, UOSInt utf16Cnt)
{
	UOSInt charCnt = Text::StrUTF16_UTF8CntC(s, utf16Cnt);
	STRINGBUILDER_ALLOCLENG(charCnt);
	Text::StrUTF16_UTF8C(&this->v[this->leng], s, utf16Cnt);
	this->leng += charCnt;
	this->v[this->leng] = 0;
	return this;
}

Text::StringBuilderUTF8 *Text::StringBuilderUTF8::AppendUTF16BE(const UInt8 *s, UOSInt utf16Cnt)
{
	UOSInt charCnt = Text::StrUTF16BE_UTF8CntC(s, utf16Cnt);
	STRINGBUILDER_ALLOCLENG(charCnt);
	Text::StrUTF16BE_UTF8C(&this->v[this->leng], s, utf16Cnt);
	this->leng += charCnt;
	this->v[this->leng] = 0;
	return this;
}

Text::StringBuilderUTF8 *Text::StringBuilderUTF8::AppendUTF32(const UTF32Char *s)
{
	UOSInt charCnt = Text::StrUTF32_UTF8Cnt(s);
	STRINGBUILDER_ALLOCLENG(charCnt);
	Text::StrUTF32_UTF8(&this->v[this->leng], s);
	this->leng += charCnt;
	return this;
}

Text::StringBuilderUTF8 *Text::StringBuilderUTF8::AppendUTF32(const UTF32Char *s, UOSInt utf32Cnt)
{
	UOSInt charCnt = Text::StrUTF32_UTF8CntC(s, utf32Cnt);
	STRINGBUILDER_ALLOCLENG(charCnt);
	Text::StrUTF32_UTF8C(&this->v[this->leng], s, utf32Cnt);
	this->leng += charCnt;
	this->v[this->leng] = 0;
	return this;
}

Text::StringBuilderUTF8 *Text::StringBuilderUTF8::AppendSlow(const UTF8Char *s)
{
	if (s == 0)
	{
		return this;
	}
	UOSInt len = Text::StrCharCnt(s);
	if (len > 0)
	{
		STRINGBUILDER_ALLOCLENG(len);
		this->leng = (UOSInt)(Text::StrConcatC(&this->v[this->leng], s, len) - this->v);
	}
	return this;
}

Text::StringBuilderUTF8 *Text::StringBuilderUTF8::AppendS(const UTF8Char *s, UOSInt maxLen)
{
	if (maxLen > 0)
	{
		STRINGBUILDER_ALLOCLENG(maxLen);
		this->leng = (UOSInt)(Text::StrConcatS(&this->v[this->leng], s, maxLen) - this->v);
	}
	return this;
}

Text::StringBuilderUTF8 *Text::StringBuilderUTF8::AppendChar(UTF32Char c, UOSInt repCnt)
{
	UTF8Char oc[6];
	UTF8Char *buffEnd;
	if (c < 0x80)
	{
		STRINGBUILDER_ALLOCLENG(repCnt);
		buffEnd = &this->v[this->leng];
		UTF8Char b = (UInt8)c;
		while (repCnt-- > 0)
		{
			buffEnd[0] = b;
			buffEnd += 1;
		}
	}
	else if (c < 0x800)
	{
		STRINGBUILDER_ALLOCLENG(2 * repCnt);
		buffEnd = &this->v[this->leng];
		oc[0] = (UTF8Char)(0xc0 | (c >> 6));
		oc[1] = (UTF8Char)(0x80 | (c & 0x3f));
		UInt16 b = ReadNUInt16(oc);
		while (repCnt-- > 0)
		{
			WriteNUInt16(buffEnd, b);
			buffEnd += 2;
		}
	}
	else if (c < 0x10000)
	{
		STRINGBUILDER_ALLOCLENG(3 * repCnt);
		buffEnd = &this->v[this->leng];
		oc[0] = (UTF8Char)(0xe0 | (c >> 12));
		oc[1] = (UTF8Char)(0x80 | ((c >> 6) & 0x3f));
		oc[2] = (UTF8Char)(0x80 | (c & 0x3f));
		UInt16 b = ReadNUInt16(oc);
		while (repCnt-- > 0)
		{
			WriteNUInt16(buffEnd, b);
			buffEnd[2] = oc[2];
			buffEnd += 3;
		}
	}
	else if (c < 0x200000)
	{
		STRINGBUILDER_ALLOCLENG(4 * repCnt);
		buffEnd = &this->v[this->leng];
		oc[0] = (UTF8Char)(0xf0 | (c >> 18));
		oc[1] = (UTF8Char)(0x80 | ((c >> 12) & 0x3f));
		oc[2] = (UTF8Char)(0x80 | ((c >> 6) & 0x3f));
		oc[3] = (UTF8Char)(0x80 | (c & 0x3f));
		UInt32 b = ReadNUInt32(oc);
		while (repCnt-- > 0)
		{
			WriteNUInt32(buffEnd, b);
			buffEnd += 4;
		}
	}
	else if (c < 0x4000000)
	{
		STRINGBUILDER_ALLOCLENG(5 * repCnt);
		buffEnd = &this->v[this->leng];
		oc[0] = (UTF8Char)(0xf8 | (c >> 24));
		oc[1] = (UTF8Char)(0x80 | ((c >> 18) & 0x3f));
		oc[2] = (UTF8Char)(0x80 | ((c >> 12) & 0x3f));
		oc[3] = (UTF8Char)(0x80 | ((c >> 6) & 0x3f));
		oc[4] = (UTF8Char)(0x80 | (c & 0x3f));
		UInt32 b = ReadNUInt32(oc);
		while (repCnt-- > 0)
		{
			WriteNUInt32(buffEnd, b);
			buffEnd[4] = oc[4];
			buffEnd += 5;
		}
	}
	else
	{
		STRINGBUILDER_ALLOCLENG(6 * repCnt);
		buffEnd = &this->v[this->leng];
		oc[0] = (UTF8Char)(0xfc | (c >> 30));
		oc[1] = (UTF8Char)(0x80 | ((c >> 24) & 0x3f));
		oc[2] = (UTF8Char)(0x80 | ((c >> 18) & 0x3f));
		oc[3] = (UTF8Char)(0x80 | ((c >> 12) & 0x3f));
		oc[4] = (UTF8Char)(0x80 | ((c >> 6) & 0x3f));
		oc[5] = (UTF8Char)(0x80 | (c & 0x3f));
		UInt32 b1 = ReadNUInt32(oc);
		UInt16 b2 = ReadNUInt16(&oc[4]);
		while (repCnt-- > 0)
		{
			WriteNUInt32(buffEnd, b1);
			WriteNUInt32(&buffEnd[4], b2);
			buffEnd += 6;
		}
	}
	buffEnd[0] = 0;
	this->leng = (UOSInt)(buffEnd - this->v);
	return this;
}

Text::StringBuilderUTF8 *Text::StringBuilderUTF8::AppendCSV(const UTF8Char **sarr, UOSInt nStr)
{
	NotNullPtr<Text::String> s;
	UOSInt i;
	i = 0;
	while (i < nStr)
	{
		s = Text::String::NewCSVRec(sarr[i]);
		if (i > 0)
			this->AppendUTF8Char(',');
		this->AppendC(s->v, s->leng);
		s->Release();
		i++;
	}
	return this;
}

Text::StringBuilderUTF8 *Text::StringBuilderUTF8::AppendToUpper(const UTF8Char *s, UOSInt len)
{
	STRINGBUILDER_ALLOCLENG(len);
	this->leng = (UOSInt)(Text::StrToUpperC(&this->v[this->leng], s, len) - this->v);
	return this;
}

Text::StringBuilderUTF8 *Text::StringBuilderUTF8::AppendToLower(const UTF8Char *s, UOSInt len)
{
	STRINGBUILDER_ALLOCLENG(len);
	this->leng = (UOSInt)(Text::StrToLowerC(&this->v[this->leng], s, len) - this->v);
	return this;
}

Text::StringBuilderUTF8 *Text::StringBuilderUTF8::RemoveANSIEscapes()
{
	this->leng = (UOSInt)(Text::StrRemoveANSIEscapes(this->v) - this->v);
	return this;
}
