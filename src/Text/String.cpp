#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Sync/Interlocked.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"
#include "Text/String.h"

//#define MEMDEBUG

Text::String Text::String::emptyStr = {0, 1, 0};

Text::String *Text::String::New(const UTF8Char *str)
{
	if (str == 0) return 0;
	UOSInt len = Text::StrCharCnt(str);
	Text::String *s = (Text::String*)MAlloc(len + sizeof(String));
	s->leng = len;
	s->cnt = 1;
	MemCopyNO(s->v, str, len + 1);
	return s;
}

Text::String *Text::String::New(const UTF8Char *str, UOSInt len)
{
	Text::String *s = (Text::String*)MAlloc(len + sizeof(String));
	s->leng = len;
	s->cnt = 1;
	MemCopyNO(s->v, str, len);
	s->v[len] = 0;
	return s;
}

Text::String *Text::String::New(UOSInt len)
{
	Text::String *s = (Text::String*)MAlloc(len + sizeof(String));
	s->leng = len;
	s->cnt = 1;
	s->v[0] = 0;
	return s;
}

Text::String *Text::String::New(const UTF16Char *str)
{
	UOSInt charCnt = Text::StrUTF16_UTF8Cnt(str);
	Text::String *s = New(charCnt);
	Text::StrUTF16_UTF8(s->v, str);
	return s;
}

Text::String *Text::String::New(const UTF32Char *str)
{
	UOSInt charCnt = Text::StrUTF32_UTF8Cnt(str);
	Text::String *s = New(charCnt);
	Text::StrUTF32_UTF8(s->v, str);
	return s;
}

Text::String *Text::String::NewCSVRec(const UTF8Char *str)
{
	UOSInt len = 2;
	UTF8Char c;
	const UTF8Char *sptr = str;
	UTF8Char *sptr2;
	while ((c = *sptr++) != 0)
	{
		if (c == '"')
		{
			len += 2;
		}
		else
		{
			len += 1;
		}
	}
	Text::String *s = New(len);
	sptr2 = s->v;
	*sptr2++ = '"';
	while ((c = *str++) != 0)
	{
		if (c == '"')
		{
			*sptr2++ = '"';
			*sptr2++ = '"';
		}
		else
		{
			*sptr2++ = c;
		}
	}
	*sptr2++ = '"';
	*sptr2 = 0;
	return s;
}

Text::String *Text::String::NewEmpty()
{
	return emptyStr.Clone();
}

Text::String *Text::String::OrEmpty(Text::String *s)
{
	if (s) return s;
	return emptyStr.Clone();
}

void Text::String::Release()
{
	this->cnt--;
	if (this->cnt == 0)
	{
		MemFree(this);
	}
}

Text::String *Text::String::Clone()
{
#if defined(MEMDEBUG)
	return New(this->v, this->leng);
#else
	this->cnt++;
	return this;
#endif
}

UTF8Char *Text::String::ConcatTo(UTF8Char *sbuff)
{
	return Text::StrConcatC(sbuff, this->v, this->leng);
}

Bool Text::String::Equals(const UTF8Char *s)
{
	return Text::StrEquals(this->v, s);
}

Bool Text::String::Equals(Text::String *s)
{
	return this->leng == s->leng && Text::StrEquals(this->v, s->v);
}

Bool Text::String::EqualsICase(const UTF8Char *s)
{
	return Text::StrEqualsICase(this->v, s);
}

Bool Text::String::StartsWith(const UTF8Char *s)
{
	return Text::StrStartsWith(this->v, s);
}

Bool Text::String::EndsWith(const UTF8Char *s)
{
	UOSInt l = Text::StrCharCnt(s);
	if (l > this->leng)
	{
		return false;
	}
	return Text::StrEquals(&this->v[this->leng - l], s);
}

Bool Text::String::HasUpperCase()
{
	return Text::StrHasUpperCase(this->v);
}

Text::String *Text::String::ToLower()
{
	if (this->HasUpperCase())
	{
		Text::String *s = Text::String::New(this->leng);
		Text::StrToLower(s->v, this->v);
		return s;
	}
	else
	{
		return this->Clone();
	}
}

UOSInt Text::String::IndexOf(const UTF8Char *s)
{
	return Text::StrIndexOf(this->v, s);
}

UOSInt Text::String::IndexOf(UTF8Char c)
{
	return Text::StrIndexOf(this->v, c);
}

OSInt Text::String::CompareTo(String *s)
{
	return Text::StrCompare(this->v, s->v);
}

OSInt Text::String::CompareTo(const UTF8Char *s)
{
	return Text::StrCompare(this->v, s);
}

Int32 Text::String::ToInt32()
{
	return Text::StrToInt32(this->v);
}

Int64 Text::String::ToInt64()
{
	return Text::StrToInt64(this->v);
}

UInt64 Text::String::ToUInt64()
{
	return Text::StrToUInt64(this->v);
}

Double Text::String::ToDouble()
{
	return Text::StrToDouble(this->v);
}

Bool Text::String::ToInt32(Int32 *outVal)
{
	return Text::StrToInt32(this->v, outVal);
}

Bool Text::String::ToDouble(Double *outVal)
{
	return Text::StrToDouble(this->v, outVal);
}

Double Text::String::MatchRating(Text::String *s)
{
	if (this->IndexOf(s->v) != INVALID_INDEX)
	{
		return Math::UOSInt2Double(s->leng) / Math::UOSInt2Double(this->leng);
	}
	else
	{
		return 0.0;
	}
}

Double Text::String::MatchRating(const UTF8Char *targetStr, UOSInt strLen)
{
	if (this->IndexOf(targetStr) != INVALID_INDEX)
	{
		return Math::UOSInt2Double(strLen) / Math::UOSInt2Double(this->leng);
	}
	else
	{
		return 0.0;
	}
}

Text::String::~String()
{
}