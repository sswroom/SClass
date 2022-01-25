#ifndef _SM_TEXT_STRINGBASE
#define _SM_TEXT_STRINGBASE
#include "Data/ByteTool.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

namespace Text
{
	template <typename T> struct StringBase
	{
		T *v;
		UOSInt leng;

		UTF8Char *ConcatTo(UTF8Char *sbuff);
		UTF8Char *ConcatWith(UTF8Char *sbuff, const UTF8Char *s1, UOSInt len1);
		UTF8Char *ConcatToS(UTF8Char *sbuff, UOSInt buffSize);
		Bool Equals(StringBase<UTF8Char> *s);
		Bool Equals(const UTF8Char *s, UOSInt len);
		Bool EqualsICase(StringBase<UTF8Char> *s);
		Bool EqualsICase(const UTF8Char *s, UOSInt len);
		Bool StartsWith(StringBase<UTF8Char> *s);
		Bool StartsWith(const UTF8Char *s, UOSInt len);
		Bool StartsWith(UOSInt startIndex, const UTF8Char *s, UOSInt len);
		Bool StartsWithICase(const UTF8Char *s, UOSInt len);
		Bool EndsWith(UTF8Char c);
		Bool EndsWith(const UTF8Char *s, UOSInt len);
		Bool EndsWithICase(const UTF8Char *s, UOSInt len);
		Bool HasUpperCase();
		Bool ContainChars(const UTF8Char *chars);
		UOSInt IndexOf(const UTF8Char *s, UOSInt len);
		UOSInt IndexOf(StringBase<UTF8Char> *s);
		UOSInt IndexOf(StringBase<const UTF8Char> *s);
		UOSInt IndexOf(const UTF8Char *s, UOSInt len, UOSInt startIndex);
		UOSInt IndexOf(UTF8Char c);
		UOSInt IndexOfICase(const UTF8Char *s, UOSInt len);
		UOSInt LastIndexOf(UTF8Char c);
		OSInt CompareTo(StringBase<UTF8Char> *s);
		OSInt CompareTo(const UTF8Char *s);
		OSInt CompareToICase(StringBase<UTF8Char> *s);
		OSInt CompareToICase(const UTF8Char *s);
		OSInt CompareToFast(const UTF8Char *s, UOSInt len);

		Int32 ToInt32();
		UInt32 ToUInt32();
		Int64 ToInt64();
		UInt64 ToUInt64();
		OSInt ToOSInt();
		UOSInt ToUOSInt();
		Double ToDouble();
		Bool ToUInt8(UInt8 *outVal);
		Bool ToInt16(Int16 *outVal);
		Bool ToUInt16(UInt16 *outVal);
		Bool ToInt32(Int32 *outVal);
		Bool ToUInt32(UInt32 *outVal);
		Bool ToInt64(Int64 *outVal);
		Bool ToUInt64(UInt64 *outVal);
		Bool ToOSInt(OSInt *outVal);
		Bool ToUOSInt(UOSInt *outVal);
		Bool ToDouble(Double *outVal);
		Bool ToUInt16S(UInt16 *outVal, UInt16 failVal);
		Bool ToUInt32S(UInt32 *outVal, UInt32 failVal);
		UOSInt Hex2Bytes(UInt8 *buff);

		Double MatchRating(StringBase<UTF8Char> *s);
		Double MatchRating(const UTF8Char *targetStr, UOSInt strLen);
	};
}


template <typename T> UTF8Char *Text::StringBase<T>::ConcatTo(UTF8Char *sbuff)
{
	REGVAR UOSInt len = this->leng;
	MemCopyNO(sbuff, this->v, len);
	sbuff += len;
	*sbuff = 0;
	return sbuff;
}

template <typename T> UTF8Char *Text::StringBase<T>::ConcatWith(UTF8Char *sbuff, const UTF8Char *s1, UOSInt len1)
{
	REGVAR UOSInt len = this->leng;
	MemCopyNO(sbuff, this->v, len);
	sbuff += len;
	MemCopyNO(sbuff, s1, len1);
	sbuff += len1;
	*sbuff = 0;
	return sbuff;
}

template <typename T> UTF8Char *Text::StringBase<T>::ConcatToS(UTF8Char *sbuff, UOSInt buffSize)
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

template <typename T> Bool Text::StringBase<T>::Equals(StringBase<UTF8Char> *s)
{
	return Equals(s->v, s->leng);
}

template <typename T> Bool Text::StringBase<T>::Equals(const UTF8Char *s, UOSInt len)
{
	return Text::StrEqualsC(this->v, this->leng, s, len);
}

template <typename T> Bool Text::StringBase<T>::EqualsICase(StringBase<UTF8Char> *s)
{
	return EqualsICase(s->v, s->leng);
}

template <typename T> Bool Text::StringBase<T>::EqualsICase(const UTF8Char *s, UOSInt len)
{
	return Text::StrEqualsICaseC(this->v, this->leng, s, len);
}

template <typename T> Bool Text::StringBase<T>::StartsWith(StringBase<UTF8Char> *s)
{
	return StartsWith(s->v, s->leng);
}

template <typename T> Bool Text::StringBase<T>::StartsWith(const UTF8Char *s, UOSInt len)
{
	return Text::StrStartsWithC(this->v, this->leng, s, len);
}

template <typename T> Bool Text::StringBase<T>::StartsWith(UOSInt startIndex, const UTF8Char *s, UOSInt len)
{
	if (startIndex > this->leng)
	{
		return false;
	}
	return Text::StrStartsWithC(&this->v[startIndex], this->leng - startIndex, s, len);
}

template <typename T> Bool Text::StringBase<T>::StartsWithICase(const UTF8Char *str2, UOSInt len)
{
	return Text::StrStartsWithICaseC(this->v, this->leng, str2, len);
}

template <typename T> Bool Text::StringBase<T>::EndsWith(UTF8Char c)
{
	REGVAR UOSInt len = this->leng;
	return len > 0 && this->v[len - 1] == c;
}

template <typename T> Bool Text::StringBase<T>::EndsWith(const UTF8Char *s, UOSInt len2)
{
	REGVAR UOSInt len1 = this->leng;
	if (len2 > len1)
	{
		return false;
	}
	return Text::StrEqualsC(&this->v[len1 - len2], len2, s, len2);
}

template <typename T> Bool Text::StringBase<T>::EndsWithICase(const UTF8Char *s, UOSInt len2)
{
	UOSInt len1 = this->leng;
	if (len2 > len1)
	{
		return false;
	}
	return Text::StrEqualsICaseC(&this->v[len1 - len2], len2, s, len2);
}

template <typename T> Bool Text::StringBase<T>::HasUpperCase()
{
	return Text::StrHasUpperCase(this->v);
}

template <typename T> Bool Text::StringBase<T>::ContainChars(const UTF8Char *chars)
{
	return Text::StrContainChars(this->v, chars);
}

template <typename T> UOSInt Text::StringBase<T>::IndexOf(const UTF8Char *s, UOSInt len)
{
	return Text::StrIndexOfC(this->v, this->leng, s, len);
}

template <typename T> UOSInt Text::StringBase<T>::IndexOf(StringBase<UTF8Char> *s)
{
	return IndexOf(s->v, s->leng);
}

template <typename T> UOSInt Text::StringBase<T>::IndexOf(StringBase<const UTF8Char> *s)
{
	return IndexOf(s->v, s->leng);
}

template <typename T> UOSInt Text::StringBase<T>::IndexOf(const UTF8Char *s, UOSInt len, UOSInt startIndex)
{
	if (startIndex >= this->leng)
	{
		return INVALID_INDEX;
	}
	UOSInt retIndex = Text::StrIndexOfC(&this->v[startIndex], this->leng - startIndex, s, len);
	if (retIndex == INVALID_INDEX)
		return INVALID_INDEX;
	return retIndex + startIndex;
}

template <typename T> UOSInt Text::StringBase<T>::IndexOf(UTF8Char c)
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

template <typename T> UOSInt Text::StringBase<T>::IndexOfICase(const UTF8Char *s, UOSInt len)
{
	return Text::StrIndexOfICase(this->v, s);
}

template <typename T> UOSInt Text::StringBase<T>::LastIndexOf(UTF8Char c)
{
	return Text::StrLastIndexOfCharC(this->v, this->leng, c);
}

template <typename T> OSInt Text::StringBase<T>::CompareTo(StringBase<UTF8Char> *s)
{
	return MyString_StrCompare(this->v, s->v);
}

template <typename T> OSInt Text::StringBase<T>::CompareTo(const UTF8Char *s)
{
	return MyString_StrCompare(this->v, s);
}

template <typename T> OSInt Text::StringBase<T>::CompareToICase(StringBase<UTF8Char> *s)
{
	return MyString_StrCompareICase(this->v, s->v);
}

template <typename T> OSInt Text::StringBase<T>::CompareToICase(const UTF8Char *s)
{
	return MyString_StrCompareICase(this->v, s);
}

template <typename T> OSInt Text::StringBase<T>::CompareToFast(const UTF8Char *str2, UOSInt len2)
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

template <typename T> Int32 Text::StringBase<T>::ToInt32()
{
	return Text::StrToInt32(this->v);
}

template <typename T> UInt32 Text::StringBase<T>::ToUInt32()
{
	return Text::StrToUInt32(this->v);
}

template <typename T> Int64 Text::StringBase<T>::ToInt64()
{
	return Text::StrToInt64(this->v);
}

template <typename T> UInt64 Text::StringBase<T>::ToUInt64()
{
	return Text::StrToUInt64(this->v);
}

template <typename T> OSInt Text::StringBase<T>::ToOSInt()
{
	return Text::StrToOSInt(this->v);
}

template <typename T> UOSInt Text::StringBase<T>::ToUOSInt()
{
	return Text::StrToUOSInt(this->v);
}

template <typename T> Double Text::StringBase<T>::ToDouble()
{
	return Text::StrToDouble(this->v);
}

template <typename T> Bool Text::StringBase<T>::ToUInt8(UInt8 *outVal)
{
	return Text::StrToUInt8(this->v, outVal);
}

template <typename T> Bool Text::StringBase<T>::ToInt16(Int16 *outVal)
{
	return Text::StrToInt16(this->v, outVal);
}

template <typename T> Bool Text::StringBase<T>::ToUInt16(UInt16 *outVal)
{
	return Text::StrToUInt16(this->v, outVal);
}

template <typename T> Bool Text::StringBase<T>::ToInt32(Int32 *outVal)
{
	return Text::StrToInt32(this->v, outVal);
}

template <typename T> Bool Text::StringBase<T>::ToUInt32(UInt32 *outVal)
{
	return Text::StrToUInt32(this->v, outVal);
}

template <typename T> Bool Text::StringBase<T>::ToInt64(Int64 *outVal)
{
	return Text::StrToInt64(this->v, outVal);
}

template <typename T> Bool Text::StringBase<T>::ToUInt64(UInt64 *outVal)
{
	return Text::StrToUInt64(this->v, outVal);
}

template <typename T> Bool Text::StringBase<T>::ToOSInt(OSInt *outVal)
{
	return Text::StrToOSInt(this->v, outVal);
}

template <typename T> Bool Text::StringBase<T>::ToUOSInt(UOSInt *outVal)
{
	return Text::StrToUOSInt(this->v, outVal);
}

template <typename T> Bool Text::StringBase<T>::ToDouble(Double *outVal)
{
	return Text::StrToDouble(this->v, outVal);
}

template <typename T> Bool Text::StringBase<T>::ToUInt16S(UInt16 *outVal, UInt16 failVal)
{
	return Text::StrToUInt16S(this->v, outVal, failVal);
}

template <typename T> Bool Text::StringBase<T>::ToUInt32S(UInt32 *outVal, UInt32 failVal)
{
	return Text::StrToUInt32S(this->v, outVal, failVal);
}

template<typename T> UOSInt Text::StringBase<T>::Hex2Bytes(UInt8 *buff)
{
	return Text::StrHex2Bytes(this->v, buff);
}

template <typename T> Double Text::StringBase<T>::MatchRating(StringBase<UTF8Char> *s)
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

template <typename T> Double Text::StringBase<T>::MatchRating(const UTF8Char *targetStr, UOSInt strLen)
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

#endif
