#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/PString.h"

UTF8Char *Text::PString::ConcatTo(UTF8Char *sbuff)
{
	REGVAR UOSInt len = this->leng;
	MemCopyNO(sbuff, this->v, len);
	sbuff += len;
	*sbuff = 0;
	return sbuff;
}

UTF8Char *Text::PString::ConcatToS(UTF8Char *sbuff, UOSInt buffSize)
{
	if (buffSize > this->leng)
	{
		MemCopyNO(sbuff, this->v, this->leng);
		sbuff += this->leng;
		*sbuff = 0;
		return sbuff;
	}
	else
	{
		buffSize--;
		MemCopyNO(sbuff, this->v, buffSize);
		sbuff[buffSize] = 0;
		return &sbuff[buffSize];
	}
}

Bool Text::PString::Equals(const UTF8Char *s, UOSInt len)
{
	return Text::StrEqualsC(this->v, this->leng, s, len);
}

Bool Text::PString::Equals(Text::PString *s)
{
	return Text::StrEqualsC(this->v, this->leng, s->v, s->leng);
}

Bool Text::PString::EqualsICase(const UTF8Char *s, UOSInt len)
{
	return Text::StrEqualsICaseC(this->v, this->leng, s, len);
}

Bool Text::PString::EqualsICase(Text::PString *s)
{
	return Text::StrEqualsICaseC(this->v, this->leng, s->v, s->leng);
}

Bool Text::PString::StartsWith(Text::PString *s)
{
	return Text::StrStartsWithC(this->v, this->leng, s->v, s->leng);
}

Bool Text::PString::StartsWith(const UTF8Char *s, UOSInt len)
{
	return Text::StrStartsWithC(this->v, this->leng, s, len);
}

Bool Text::PString::StartsWith(UOSInt startIndex, const UTF8Char *s, UOSInt len)
{
	if (startIndex > this->leng)
	{
		return false;
	}
	return Text::StrStartsWithC(&this->v[startIndex], this->leng - startIndex, s, len);
}

Bool Text::PString::StartsWithICase(const UTF8Char *str2, UOSInt len)
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

Bool Text::PString::EndsWith(UTF8Char c)
{
	REGVAR UOSInt len = this->leng;
	return len > 0 && this->v[len - 1] == c;
}

Bool Text::PString::EndsWith(const UTF8Char *s, UOSInt len2)
{
	REGVAR UOSInt len1 = this->leng;
	if (len2 > len1)
	{
		return false;
	}
	return Text::StrEqualsC(&this->v[len1 - len2], len2, s, len2);
}

Bool Text::PString::EndsWithICase(const UTF8Char *s, UOSInt len2)
{
	UOSInt len1 = this->leng;
	if (len2 > len1)
	{
		return false;
	}
	return Text::StrEqualsICaseC(&this->v[len1 - len2], len2, s, len2);
}

Bool Text::PString::HasUpperCase()
{
	return Text::StrHasUpperCase(this->v);
}

Bool Text::PString::ContainChars(const UTF8Char *chars)
{
	return Text::StrContainChars(this->v, chars);
}

UOSInt Text::PString::IndexOf(const UTF8Char *s, UOSInt len)
{
	return Text::StrIndexOfC(this->v, this->leng, s, len);
}

UOSInt Text::PString::IndexOf(UTF8Char c)
{
	REGVAR const UTF8Char *ptr = this->v;
	REGVAR UOSInt len1 = this->leng;
	REGVAR UInt16 c2;
	while (len1 >= 2)
	{
		c2 = ReadUInt16(ptr);
		if ((UTF8Char)(c2 & 0xff) == c)
			return (UOSInt)(ptr - this->v);
		if ((UTF8Char)(c2 >> 8) == c)
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

UOSInt Text::PString::IndexOfICase(const UTF8Char *s)
{
	return Text::StrIndexOfICase(this->v, s);
}

UOSInt Text::PString::LastIndexOf(UTF8Char c)
{
	REGVAR UOSInt l = this->leng;
	while (l-- > 0)
	{
		if (this->v[l] == c)
			return l;
	}
	return INVALID_INDEX;
}

OSInt Text::PString::CompareTo(PString *s)
{
	return MyString_StrCompare(this->v, s->v);
}

OSInt Text::PString::CompareTo(const UTF8Char *s)
{
	return MyString_StrCompare(this->v, s);
}

OSInt Text::PString::CompareToICase(Text::PString *s)
{
	return MyString_StrCompareICase(this->v, s->v);
}

OSInt Text::PString::CompareToICase(const UTF8Char *s)
{
	return MyString_StrCompareICase(this->v, s);
}

OSInt Text::PString::CompareToFast(const UTF8Char *str2, UOSInt len2)
{
	const UTF8Char *s0 = this->v;
	UOSInt len1 = this->leng;
	OSInt defRet;
	if (len1 > len2)
	{
		defRet = 1;
	}
	else if (len1 == len2)
	{
		defRet = 0;
	}
	else
	{
		defRet = -1;
		len2 = len1;
	}
	while (len2 >= 4)
	{
		REGVAR UInt32 v1 = ReadMUInt32(s0);
		REGVAR UInt32 v2 = ReadMUInt32(str2);
		if (v1 > v2)
		{
			return 1;
		}
		else if (v1 < v2)
		{
			return -1;
		}
		len2 -= 4;
		s0 += 4;
		str2 += 4;
	}
	while (len2 > 0)
	{
		REGVAR UTF8Char c1 = *s0;
		REGVAR UTF8Char c2 = *str2;
		if (c1 > c2)
		{
			return 1;
		}
		else if (c1 < c2)
		{
			return -1;
		}
		len2--;
		s0++;
		str2++;
	}
	return defRet;
}

void Text::PString::RTrim()
{
	UOSInt len = this->leng;
	while (len > 0)
	{
		UTF8Char c = this->v[len - 1];
		if (c == ' ' || c == '\t')
		{
			len--;
		}
		else
		{
			break;
		}
	}
	this->v[len] = 0;
	this->leng = len;
}

Int32 Text::PString::ToInt32()
{
	return Text::StrToInt32(this->v);
}

UInt32 Text::PString::ToUInt32()
{
	return Text::StrToUInt32(this->v);
}

Int64 Text::PString::ToInt64()
{
	return Text::StrToInt64(this->v);
}

UInt64 Text::PString::ToUInt64()
{
	return Text::StrToUInt64(this->v);
}

OSInt Text::PString::ToOSInt()
{
	return Text::StrToOSInt(this->v);
}

UOSInt Text::PString::ToUOSInt()
{
	return Text::StrToUOSInt(this->v);
}

Double Text::PString::ToDouble()
{
	return Text::StrToDouble(this->v);
}

Bool Text::PString::ToUInt8(UInt8 *outVal)
{
	return Text::StrToUInt8(this->v, outVal);
}

Bool Text::PString::ToInt16(Int16 *outVal)
{
	return Text::StrToInt16(this->v, outVal);
}

Bool Text::PString::ToUInt16(UInt16 *outVal)
{
	return Text::StrToUInt16(this->v, outVal);
}

Bool Text::PString::ToInt32(Int32 *outVal)
{
	return Text::StrToInt32(this->v, outVal);
}

Bool Text::PString::ToUInt32(UInt32 *outVal)
{
	return Text::StrToUInt32(this->v, outVal);
}

Bool Text::PString::ToInt64(Int64 *outVal)
{
	return Text::StrToInt64(this->v, outVal);
}

Bool Text::PString::ToUInt64(UInt64 *outVal)
{
	return Text::StrToUInt64(this->v, outVal);
}

Bool Text::PString::ToDouble(Double *outVal)
{
	return Text::StrToDouble(this->v, outVal);
}

Bool Text::PString::ToUInt16S(UInt16 *outVal, UInt16 failVal)
{
	return Text::StrToUInt16S(this->v, outVal, failVal);
}

Double Text::PString::MatchRating(Text::PString *s)
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

Double Text::PString::MatchRating(const UTF8Char *targetStr, UOSInt strLen)
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

UOSInt Text::StrSplitP(PString *strs, UOSInt maxStrs, UTF8Char *strToSplit, UOSInt strLen, UTF8Char splitChar)
{
	UOSInt i = 0;
	UTF8Char c;
	strs[i].v = strToSplit;
	strs[i].leng = strLen;
	i++;
	while (i < maxStrs)
	{
		c = *strToSplit++;
		if (c == 0)
			break;
		if (c == splitChar)
		{
			strs[i].leng = strs[i - 1].leng;
			strs[i - 1].leng = (UOSInt)(strToSplit - strs[i - 1].v - 1);
			strs[i].v = strToSplit;
			strs[i].leng -= strs[i - 1].leng + 1;
			strToSplit[-1] = 0;
			i++;
		}
	}
	return i;
}

UOSInt Text::StrSplitTrimP(Text::PString *strs, UOSInt maxStrs, UTF8Char *strToSplit, UOSInt strLen, UTF8Char splitChar)
{
	UOSInt i = 0;
	UTF8Char c;
	UTF8Char *lastPtr;
	UTF8Char *thisPtr;
	while (*strToSplit == ' ' || *strToSplit == '\r' || *strToSplit == '\n' || *strToSplit == '\t')
	{
		strToSplit++;
		strLen--;
	}
	strs[i].v = lastPtr = strToSplit;
	strs[i].leng = strLen;
	i++;
	while (i < maxStrs)
	{
		c = *strToSplit++;
		if (c == 0)
		{
			thisPtr = strToSplit - 1;
			while (lastPtr < thisPtr)
			{
				c = thisPtr[-1];
				if (c == ' ' || c == '\r' || c == '\n' || c == '\t')
				{
					*--thisPtr = 0;
				}
				else
				{
					break;
				}
			}
			strs[i - 1].leng = (UOSInt)(thisPtr - strs[i - 1].v);
			break;
		}
		if (c == splitChar)
		{
			strToSplit[-1] = 0;

			thisPtr = strToSplit - 1;
			while (lastPtr < thisPtr)
			{
				c = thisPtr[-1];
				if (c == ' ' || c == '\r' || c == '\n' || c == '\t')
					*--thisPtr = 0;
				else
					break;
			}
			strs[i].leng = strs[i - 1].leng;
			strs[i - 1].leng = (UOSInt)(thisPtr - strs[i - 1].v);

			while (*strToSplit == ' ' || *strToSplit == '\r' || *strToSplit == '\n' || *strToSplit == '\t')
				strToSplit++;
			strs[i].v = lastPtr = strToSplit;
			strs[i].leng = strs[i].leng - (UOSInt)(strs[i].v - strs[i - 1].v);
			i++;
		}
	}
	return i;
}

UOSInt Text::StrSplitLineP(PString *strs, UOSInt maxStrs, UTF8Char *strToSplit, UOSInt strLen)
{
	UOSInt i = 0;
	UTF8Char c;
	strs[i].v = strToSplit;
	strs[i].leng = strLen;
	i++;
	while (i < maxStrs)
	{
		c = *strToSplit++;
		if (c == 0)
			break;
		if (c == 13)
		{
			strToSplit[-1] = 0;
			strs[i].leng = strs[i - 1].leng;
			strs[i - 1].leng = (UOSInt)(&strToSplit[-1] - strs[i - 1].v);
			if (*strToSplit == 10)
			{
				strToSplit++;
				strs[i].v = strToSplit;
				strs[i].leng -= strs[i - 1].leng - 2;
			}
			else
			{
				strs[i].v = strToSplit;
				strs[i].leng -= strs[i - 1].leng - 1;
			}
			i++;
		}
		else if (c == 10)
		{
			strToSplit[-1] = 0;
			strs[i].leng = strs[i - 1].leng;
			strs[i - 1].leng = (UOSInt)(&strToSplit[-1] - strs[i - 1].v);
			strs[i].v = strToSplit;
			strs[i].leng -= strs[i - 1].leng - 1;
			i++;
		}
	}
	return i;
}

UOSInt Text::StrSplitWSP(Text::PString *strs, UOSInt maxStrs, UTF8Char *strToSplit, UOSInt strLen)
{
	UOSInt i = 0;
	UTF8Char c;
	strs[0].v = strToSplit;
	while (true)
	{
		c = *strToSplit++;
		if (c == 0)
		{
			return 0;
		}
		if (c != 32 && c != '\t')
		{
			strs[0].leng = strLen - (UOSInt)(strToSplit - strs[0].v - 1);
			strs[0].v = strToSplit;
			i = 1;
			break;
		}
	}
	while (i < maxStrs)
	{
		c = *strToSplit++;
		if (c == 0)
			break;
		if (c == 32 || c == '\t')
		{
			strToSplit[-1] = 0;
			strs[i].leng = strs[i - 1].leng;
			strs[i - 1].leng = (UOSInt)(strToSplit - strs[i - 1].v - 1);
			while (true)
			{
				c = *strToSplit++;
				if (c == 0)
				{
					return i;
				}
				if (c != 32 && c != '\t')
				{
					strs[i].v = strToSplit - 1;
					strs[i].leng = strs[i].leng - (UOSInt)(strToSplit - strs[i - 1].v - 1);
					i++;
					break;
				}
			}
		}
	}
	return i;
}

UOSInt Text::StrCSVSplitP(Text::PString *strs, UOSInt maxStrs, UTF8Char *strToSplit)
{
	Bool quoted = false;
	Bool first = true;
	UOSInt i = 0;
	UTF8Char *strCurr;
	UTF8Char c;
	strs[i++].v = strCurr = strToSplit;
	while (i < maxStrs)
	{
		c = *strToSplit++;
		if (c == 0)
		{
			*strCurr = 0;
			strs[i - 1].leng = (UOSInt)(strCurr - strs[i - 1].v);
			break;
		}
		if (c == '"')
		{
			if (!quoted)
			{
				quoted = true;
				first = false;
			}
			else if (*strToSplit == '"')
			{
				strToSplit++;
				*strCurr++ = '"';
				first = false;
			}
			else
			{
				quoted = false;
			}
		}
		else if (c == ',' && !quoted)
		{
			*strCurr = 0;
			strs[i - 1].leng = (UOSInt)(strCurr - strs[i - 1].v);
			strs[i++].v = strCurr = strToSplit;
			first = true;
		}
		else
		{
			if (c == ' ' && first)
			{
			}
			else
			{
				*strCurr++ = c;
				first = false;
			}
		}
	}
	return i;
}
