#include "Stdafx.h"
#include "MemTool.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Math/Math.h"
#include "Sync/Interlocked.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"
#include "Text/String.h"

//#define THREADSAFE
//#define MEMDEBUG

Text::String Text::String::emptyStr = {0, 1048576, 0};

Text::String *Text::String::NewOrNull(const UTF8Char *str)
{
	if (str == 0) return 0;
	UOSInt len = Text::StrCharCnt(str);
	Text::String *s = (Text::String*)MAlloc(len + sizeof(String));
	s->leng = len;
	s->cnt = 1;
	MemCopyNO(s->v, str, len + 1);
	return s;
}

Text::String *Text::String::NewNotNull(const UTF8Char *str)
{
	UOSInt len = Text::StrCharCnt(str);
	Text::String *s = (Text::String*)MAlloc(len + sizeof(String));
	s->leng = len;
	s->cnt = 1;
	MemCopyNO(s->v, str, len + 1);
	return s;
}

Text::String *Text::String::New(const UTF8Char *str, UOSInt len)
{
	if (len == 0) return NewEmpty();
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

Text::String *Text::String::NewOrNull(const UTF16Char *str)
{
	if (str == 0) return 0;
	UOSInt charCnt = Text::StrUTF16_UTF8Cnt(str);
	Text::String *s = New(charCnt);
	Text::StrUTF16_UTF8(s->v, str);
	return s;
}

Text::String *Text::String::NewNotNull(const UTF16Char *str)
{
	UOSInt charCnt = Text::StrUTF16_UTF8Cnt(str);
	Text::String *s = New(charCnt);
	Text::StrUTF16_UTF8(s->v, str);
	return s;
}

Text::String *Text::String::New(const UTF16Char *str, UOSInt len)
{
	if (len == 0) return NewEmpty();
	UOSInt charCnt = Text::StrUTF16_UTF8CntC(str, len);
	Text::String *s = New(charCnt);
	Text::StrUTF16_UTF8C(s->v, str, len);
	s->v[charCnt] = 0;
	return s;
}

Text::String *Text::String::NewOrNull(const UTF32Char *str)
{
	if (str == 0) return 0;
	UOSInt charCnt = Text::StrUTF32_UTF8Cnt(str);
	Text::String *s = New(charCnt);
	Text::StrUTF32_UTF8(s->v, str);
	return s;
}

Text::String *Text::String::NewNotNull(const UTF32Char *str)
{
	UOSInt charCnt = Text::StrUTF32_UTF8Cnt(str);
	Text::String *s = New(charCnt);
	Text::StrUTF32_UTF8(s->v, str);
	return s;
}

Text::String *Text::String::New(const UTF32Char *str, UOSInt len)
{
	if (len == 0) return NewEmpty();
	UOSInt charCnt = Text::StrUTF32_UTF8CntC(str, len);
	Text::String *s = New(charCnt);
	Text::StrUTF32_UTF8C(s->v, str, len);
	s->v[charCnt] = 0;
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
#if defined(THREADSAFE)
#if _OSINT_SIZE == 64
	Interlocked_DecrementU64(&this->cnt);
#else
	Interlocked_DecrementU32(&this->cnt);
#endif
	if (this->cnt == 0)
	{
		MemFree(this);
	}
#else
	this->cnt--;
	if (this->cnt == 0)
	{
		MemFree(this);
	}
#endif
}

Text::String *Text::String::Clone()
{
#if defined(MEMDEBUG)
	return New(this->v, this->leng);
#elif defined(THREADSAFE)
	#if _OSINT_SIZE == 64
	Interlocked_IncrementU64(&this->cnt);
	#else
	Interlocked_IncrementU32(&this->cnt);
	#endif
	return this;
#else
	this->cnt++;
	return this;
#endif
}

UTF8Char *Text::String::ConcatTo(UTF8Char *sbuff)
{
	MemCopyNO(sbuff, this->v, this->leng);
	sbuff[this->leng] = 0;
	return &sbuff[this->leng];
}

UTF8Char *Text::String::ConcatToS(UTF8Char *sbuff, UOSInt buffSize)
{
	if (buffSize > this->leng)
	{
		MemCopyNO(sbuff, this->v, this->leng);
		sbuff[this->leng] = 0;
		return &sbuff[this->leng];
	}
	else
	{
		buffSize--;
		MemCopyNO(sbuff, this->v, buffSize);
		sbuff[buffSize] = 0;
		return &sbuff[buffSize];
	}
}

Bool Text::String::Equals(const UTF8Char *s, UOSInt len)
{
	return Text::StrEqualsC(this->v, this->leng, s, len);
}

Bool Text::String::Equals(Text::String *s)
{
	return Text::StrEqualsC(this->v, this->leng, s->v, s->leng);
}

Bool Text::String::EqualsICase(const UTF8Char *s, UOSInt len)
{
	return Text::StrEqualsICaseC(this->v, this->leng, s, len);
}

Bool Text::String::EqualsICase(Text::String *s)
{
	return Text::StrEqualsICaseC(this->v, this->leng, s->v, s->leng);
}

Bool Text::String::StartsWith(Text::String *s)
{
	return Text::StrStartsWithC(this->v, this->leng, s->v, s->leng);
}

Bool Text::String::StartsWith(const UTF8Char *s, UOSInt len)
{
	return Text::StrStartsWithC(this->v, this->leng, s, len);
}

Bool Text::String::StartsWith(UOSInt startIndex, const UTF8Char *s, UOSInt len)
{
	if (startIndex > this->leng)
	{
		return false;
	}
	return Text::StrStartsWithC(&this->v[startIndex], this->leng - startIndex, s, len);
}

Bool Text::String::StartsWithICase(const UTF8Char *str2, UOSInt len)
{
	if (this->leng < len)
	{
		return false;
	}
	REGVAR UInt8 *upperArr = MyString_StrUpperArr;
	const UTF8Char *str1 = this->v;
	while (len >= 4)
	{
		REGVAR UInt32 v1 = ReadNUInt32(str1);
		REGVAR UInt32 v2 = ReadNUInt32(str2);
		if (upperArr[v1 & 0xff] != upperArr[v2 & 0xff])
			return false;
		if (upperArr[(v1 >> 8) & 0xff] != upperArr[(v2 >> 8) & 0xff])
			return false;
		if (upperArr[(v1 >> 16) & 0xff] != upperArr[(v2 >> 16) & 0xff])
			return false;
		if (upperArr[(v1 >> 24)] != upperArr[(v2 >> 24)])
			return false;
		str1 += 4;
		str2 += 4;
		len -= 4;
	}
	if (len >= 2)
	{
		REGVAR UInt16 v1 = ReadNUInt16(str1);
		REGVAR UInt16 v2 = ReadNUInt16(str2);
		if (upperArr[v1 & 0xff] != upperArr[v2 & 0xff] ||
			upperArr[(v1 >> 8)] != upperArr[(v2 >> 8)])
			return false;
		str1 += 2;
		str2 += 2;
		len -= 2;
	}
	if (len > 0)
	{
		return upperArr[*str1] == upperArr[*str2];
	}
	return true;
}

Bool Text::String::EndsWith(UTF8Char c)
{
	return this->leng > 0 && this->v[this->leng - 1] == c;
}

Bool Text::String::EndsWith(const UTF8Char *s, UOSInt len)
{
	if (len > this->leng)
	{
		return false;
	}
	return Text::StrEqualsC(&this->v[this->leng - len], len, s, len);
}

Bool Text::String::EndsWithICase(const UTF8Char *s, UOSInt len)
{
	if (len > this->leng)
	{
		return false;
	}
	return Text::StrEqualsICaseC(&this->v[this->leng - len], len, s, len);
}

Bool Text::String::HasUpperCase()
{
	return Text::StrHasUpperCase(this->v);
}

Bool Text::String::ContainChars(const UTF8Char *chars)
{
	return Text::StrContainChars(this->v, chars);
}

Text::String *Text::String::ToLower()
{
	if (this->HasUpperCase())
	{
		Text::String *s = Text::String::New(this->leng);
		Text::StrToLowerC(s->v, this->v, this->leng);
		return s;
	}
	else
	{
		return this->Clone();
	}
}

UOSInt Text::String::IndexOf(const UTF8Char *s, UOSInt len)
{
	return Text::StrIndexOfC(this->v, this->leng, s, len);
}

UOSInt Text::String::IndexOf(UTF8Char c)
{
	REGVAR const UTF8Char *ptr = this->v;
	REGVAR UOSInt len1 = this->leng;
	REGVAR UInt16 c2;
	while (len1 >= 2)
	{
		c2 = ReadUInt16(ptr);
		if ((c2 & 0xff) == c)
			return (UOSInt)(ptr - this->v);
		if ((c2 >> 8) == c)
			return (UOSInt)(ptr - this->v + 1);
		ptr += 2;
		len1 -= 2;
	}
	if (len1 && (*ptr == c))
	{
		return (UOSInt)(ptr - this->v);
	}
	return INVALID_INDEX;
}

UOSInt Text::String::IndexOfICase(const UTF8Char *s)
{
	return Text::StrIndexOfICase(this->v, s);
}

UOSInt Text::String::LastIndexOf(UTF8Char c)
{
	UOSInt l = this->leng;
	while (l-- > 0)
	{
		if (this->v[l] == c)
			return l;
	}
	return INVALID_INDEX;
}

OSInt Text::String::CompareTo(String *s)
{
	return MyString_StrCompare(this->v, s->v);
}

OSInt Text::String::CompareTo(const UTF8Char *s)
{
	return MyString_StrCompare(this->v, s);
}

OSInt Text::String::CompareToICase(Text::String *s)
{
	return MyString_StrCompareICase(this->v, s->v);
}

OSInt Text::String::CompareToICase(const UTF8Char *s)
{
	return MyString_StrCompareICase(this->v, s);
}

OSInt Text::String::CompareToFast(const UTF8Char *s, UOSInt len)
{
	const UTF8Char *s0 = this->v;
	OSInt defRet;
	if (this->leng > len)
	{
		defRet = 1;
	}
	else if (this->leng == len)
	{
		defRet = 0;
	}
	else
	{
		defRet = -1;
		len = this->leng;
	}
	UInt32 v1;
	UInt32 v2;
	UTF8Char c1;
	UTF8Char c2;
	while (len >= 4)
	{
		v1 = ReadMUInt32(s0);
		v2 = ReadMUInt32(s);
		if (v1 > v2)
		{
			return 1;
		}
		else if (v1 < v2)
		{
			return -1;
		}
		len -= 4;
		s0 += 4;
		s += 4;
	}
	while (len > 0)
	{
		c1 = *s0;
		c2 = *s;
		if (c1 > c2)
		{
			return 1;
		}
		else if (c1 < c2)
		{
			return -1;
		}
		len--;
		s0++;
		s++;
	}
	return defRet;
}

void Text::String::RTrim()
{
	while (this->leng > 0)
	{
		UTF8Char c = this->v[this->leng - 1];
		if (c == ' ' || c == '\t')
		{
			this->leng--;
		}
		else
		{
			break;
		}
	}
	this->v[this->leng] = 0;
}

Int32 Text::String::ToInt32()
{
	return Text::StrToInt32(this->v);
}

UInt32 Text::String::ToUInt32()
{
	return Text::StrToUInt32(this->v);
}

Int64 Text::String::ToInt64()
{
	return Text::StrToInt64(this->v);
}

UInt64 Text::String::ToUInt64()
{
	return Text::StrToUInt64(this->v);
}

OSInt Text::String::ToOSInt()
{
	return Text::StrToOSInt(this->v);
}

UOSInt Text::String::ToUOSInt()
{
	return Text::StrToUOSInt(this->v);
}

Double Text::String::ToDouble()
{
	return Text::StrToDouble(this->v);
}

Bool Text::String::ToUInt8(UInt8 *outVal)
{
	return Text::StrToUInt8(this->v, outVal);
}

Bool Text::String::ToInt16(Int16 *outVal)
{
	return Text::StrToInt16(this->v, outVal);
}

Bool Text::String::ToUInt16(UInt16 *outVal)
{
	return Text::StrToUInt16(this->v, outVal);
}

Bool Text::String::ToInt32(Int32 *outVal)
{
	return Text::StrToInt32(this->v, outVal);
}

Bool Text::String::ToUInt32(UInt32 *outVal)
{
	return Text::StrToUInt32(this->v, outVal);
}

Bool Text::String::ToInt64(Int64 *outVal)
{
	return Text::StrToInt64(this->v, outVal);
}

Bool Text::String::ToUInt64(UInt64 *outVal)
{
	return Text::StrToUInt64(this->v, outVal);
}

Bool Text::String::ToDouble(Double *outVal)
{
	return Text::StrToDouble(this->v, outVal);
}

Bool Text::String::ToUInt16S(UInt16 *outVal, UInt16 failVal)
{
	return Text::StrToUInt16S(this->v, outVal, failVal);
}

Double Text::String::MatchRating(Text::String *s)
{
	if (this->IndexOf(s->v, s->leng) != INVALID_INDEX)
	{
		return UOSInt2Double(s->leng) / UOSInt2Double(this->leng);
	}
	else
	{
		return 0.0;
	}
}

Double Text::String::MatchRating(const UTF8Char *targetStr, UOSInt strLen)
{
	if (this->IndexOf(targetStr, strLen) != INVALID_INDEX)
	{
		return UOSInt2Double(strLen) / UOSInt2Double(this->leng);
	}
	else
	{
		return 0.0;
	}
}

Text::String::~String()
{
}