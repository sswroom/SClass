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

		T *GetEndPtr();

		UTF8Char *ConcatTo(UTF8Char *sbuff) const;
		UTF8Char *ConcatWith(UTF8Char *sbuff, const UTF8Char *s1, UOSInt len1) const;
		UTF8Char *ConcatToS(UTF8Char *sbuff, UOSInt buffSize) const;
		Bool Equals(StringBase<UTF8Char> *s) const;
		Bool Equals(const UTF8Char *s, UOSInt len) const;
		Bool EqualsICase(StringBase<UTF8Char> *s) const;
		Bool EqualsICase(const UTF8Char *s, UOSInt len) const;
		Bool StartsWith(UTF8Char c) const;
		Bool StartsWith(StringBase<UTF8Char> *s) const;
		Bool StartsWith(const UTF8Char *s, UOSInt len) const;
		Bool StartsWith(UOSInt startIndex, const UTF8Char *s, UOSInt len) const;
		Bool StartsWithICase(const UTF8Char *s, UOSInt len) const;
		Bool StartsWithICase(UOSInt startIndex, const UTF8Char *s, UOSInt len) const;
		Bool EndsWith(UTF8Char c) const;
		Bool EndsWith(const UTF8Char *s, UOSInt len) const;
		Bool EndsWithICase(const UTF8Char *s, UOSInt len) const;
		Bool HasUpperCase() const;
		Bool ContainChars(const UTF8Char *chars) const;
		UOSInt IndexOf(const UTF8Char *s, UOSInt len) const;
		UOSInt IndexOf(StringBase<UTF8Char> *s) const;
		UOSInt IndexOf(StringBase<const UTF8Char> *s) const;
		UOSInt IndexOf(const UTF8Char *s, UOSInt len, UOSInt startIndex) const;
		UOSInt IndexOf(UTF8Char c) const;
		UOSInt IndexOf(UTF8Char c, UOSInt startIndex) const;
		UOSInt IndexOfICase(const UTF8Char *s, UOSInt len) const;
		UOSInt IndexOfICase(StringBase<UTF8Char> *s) const;
		UOSInt LastIndexOf(UTF8Char c) const;
		OSInt CompareTo(StringBase<UTF8Char> *s) const;
		OSInt CompareTo(StringBase<const UTF8Char> s) const;
		OSInt CompareTo(const UTF8Char *s) const;
		OSInt CompareToICase(StringBase<UTF8Char> *s) const;
		OSInt CompareToICase(const UTF8Char *s) const;
		OSInt CompareToFast(StringBase<const UTF8Char> s) const;

		Int32 ToInt32() const;
		UInt32 ToUInt32() const;
		Int64 ToInt64() const;
		UInt64 ToUInt64() const;
		OSInt ToOSInt() const;
		UOSInt ToUOSInt() const;
		Double ToDouble() const;
		Bool ToUInt8(UInt8 *outVal) const;
		Bool ToInt16(Int16 *outVal) const;
		Bool ToUInt16(UInt16 *outVal) const;
		Bool ToInt32(Int32 *outVal) const;
		Bool ToUInt32(UInt32 *outVal) const;
		Bool ToInt64(Int64 *outVal) const;
		Bool ToUInt64(UInt64 *outVal) const;
		Bool ToOSInt(OSInt *outVal) const;
		Bool ToUOSInt(UOSInt *outVal) const;
		Bool ToDouble(Double *outVal) const;
		Bool ToUInt16S(UInt16 *outVal, UInt16 failVal) const;
		Bool ToUInt32S(UInt32 *outVal, UInt32 failVal) const;
		UOSInt Hex2Bytes(UInt8 *buff) const;
		UOSInt CountChar(UTF8Char c) const;
		UOSInt CountStr(const UTF8Char *s, UOSInt len) const;
		UOSInt CountStr(StringBase<UTF8Char> *s) const;
		UOSInt CountStr(const StringBase<const UTF8Char> &s) const;
		Bool Hex2UInt16(UInt16 *outVal) const;
		Bool Hex2UInt32(UInt32 *outVal) const;

		Double MatchRating(StringBase<UTF8Char> *s) const;
		Double MatchRating(const UTF8Char *targetStr, UOSInt strLen) const;

		Bool operator==(StringBase<T> s) const;
	};
}

template <typename T> T *Text::StringBase<T>::GetEndPtr()
{
	return &this->v[this->leng];
}

template <typename T> UTF8Char *Text::StringBase<T>::ConcatTo(UTF8Char *sbuff) const
{
	REGVAR UOSInt len = this->leng;
	MemCopyNO(sbuff, this->v, len);
	sbuff += len;
	*sbuff = 0;
	return sbuff;
}

template <typename T> UTF8Char *Text::StringBase<T>::ConcatWith(UTF8Char *sbuff, const UTF8Char *s1, UOSInt len1) const
{
	REGVAR UOSInt len = this->leng;
	MemCopyNO(sbuff, this->v, len);
	sbuff += len;
	MemCopyNO(sbuff, s1, len1);
	sbuff += len1;
	*sbuff = 0;
	return sbuff;
}

template <typename T> UTF8Char *Text::StringBase<T>::ConcatToS(UTF8Char *sbuff, UOSInt buffSize) const
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

template <typename T> Bool Text::StringBase<T>::Equals(StringBase<UTF8Char> *s) const
{
	return Equals(s->v, s->leng);
}

template <typename T> Bool Text::StringBase<T>::Equals(const UTF8Char *s, UOSInt len) const
{
	return Text::StrEqualsC(this->v, this->leng, s, len);
}

template <typename T> Bool Text::StringBase<T>::EqualsICase(StringBase<UTF8Char> *s) const
{
	return EqualsICase(s->v, s->leng);
}

template <typename T> Bool Text::StringBase<T>::EqualsICase(const UTF8Char *s, UOSInt len) const
{
	return Text::StrEqualsICaseC(this->v, this->leng, s, len);
}

template <typename T> Bool Text::StringBase<T>::StartsWith(UTF8Char c) const
{
	return this->v[0] == c;
}

template <typename T> Bool Text::StringBase<T>::StartsWith(StringBase<UTF8Char> *s) const
{
	return StartsWith(s->v, s->leng);
}

template <typename T> Bool Text::StringBase<T>::StartsWith(const UTF8Char *s, UOSInt len) const
{
	return Text::StrStartsWithC(this->v, this->leng, s, len);
}

template <typename T> Bool Text::StringBase<T>::StartsWith(UOSInt startIndex, const UTF8Char *s, UOSInt len) const
{
	if (startIndex > this->leng)
	{
		return false;
	}
	return Text::StrStartsWithC(&this->v[startIndex], this->leng - startIndex, s, len);
}

template <typename T> Bool Text::StringBase<T>::StartsWithICase(const UTF8Char *str2, UOSInt len) const
{
	return Text::StrStartsWithICaseC(this->v, this->leng, str2, len);
}

template <typename T> Bool Text::StringBase<T>::StartsWithICase(UOSInt startIndex, const UTF8Char *s, UOSInt len) const
{
	if (startIndex > this->leng)
	{
		return false;
	}
	return Text::StrStartsWithICaseC(&this->v[startIndex], this->leng - startIndex, s, len);
}

template <typename T> Bool Text::StringBase<T>::EndsWith(UTF8Char c) const
{
	REGVAR UOSInt len = this->leng;
	return len > 0 && this->v[len - 1] == c;
}

template <typename T> Bool Text::StringBase<T>::EndsWith(const UTF8Char *s, UOSInt len2) const
{
	REGVAR UOSInt len1 = this->leng;
	if (len2 > len1)
	{
		return false;
	}
	return Text::StrEqualsC(&this->v[len1 - len2], len2, s, len2);
}

template <typename T> Bool Text::StringBase<T>::EndsWithICase(const UTF8Char *s, UOSInt len2) const
{
	UOSInt len1 = this->leng;
	if (len2 > len1)
	{
		return false;
	}
	return Text::StrEqualsICaseC(&this->v[len1 - len2], len2, s, len2);
}

template <typename T> Bool Text::StringBase<T>::HasUpperCase() const
{
	return Text::StrHasUpperCase(this->v);
}

template <typename T> Bool Text::StringBase<T>::ContainChars(const UTF8Char *chars) const
{
	return Text::StrContainChars(this->v, chars);
}

template <typename T> UOSInt Text::StringBase<T>::IndexOf(const UTF8Char *s, UOSInt len) const
{
	return Text::StrIndexOfC(this->v, this->leng, s, len);
}

template <typename T> UOSInt Text::StringBase<T>::IndexOf(StringBase<UTF8Char> *s) const
{
	return IndexOf(s->v, s->leng);
}

template <typename T> UOSInt Text::StringBase<T>::IndexOf(StringBase<const UTF8Char> *s) const
{
	return IndexOf(s->v, s->leng);
}

template <typename T> UOSInt Text::StringBase<T>::IndexOf(const UTF8Char *s, UOSInt len, UOSInt startIndex) const
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

template <typename T> UOSInt Text::StringBase<T>::IndexOf(UTF8Char c) const
{
	return Text::StrIndexOfCharC(this->v, this->leng, c);
}

template <typename T> UOSInt Text::StringBase<T>::IndexOf(UTF8Char c, UOSInt startIndex) const
{
	if (startIndex >= this->leng)
		return INVALID_INDEX;
	UOSInt i = Text::StrIndexOfCharC(this->v + startIndex, this->leng - startIndex, c);
	if (i == INVALID_INDEX)
		return INVALID_INDEX;
	return i + startIndex;
}

template <typename T> UOSInt Text::StringBase<T>::IndexOfICase(const UTF8Char *s, UOSInt len) const
{
	return Text::StrIndexOfICase(this->v, s);
}

template <typename T> UOSInt Text::StringBase<T>::IndexOfICase(StringBase<UTF8Char> *s) const
{
	return IndexOfICase(s->v, s->leng);
}

template <typename T> UOSInt Text::StringBase<T>::LastIndexOf(UTF8Char c) const
{
	return Text::StrLastIndexOfCharC(this->v, this->leng, c);
}

template <typename T> OSInt Text::StringBase<T>::CompareTo(StringBase<UTF8Char> *s) const
{
	return MyString_StrCompare(this->v, s->v);
}

template <typename T> OSInt Text::StringBase<T>::CompareTo(StringBase<const UTF8Char> s) const
{
	return MyString_StrCompare(this->v, s.v);
}

template <typename T> OSInt Text::StringBase<T>::CompareTo(const UTF8Char *s) const
{
	return MyString_StrCompare(this->v, s);
}

template <typename T> OSInt Text::StringBase<T>::CompareToICase(StringBase<UTF8Char> *s) const
{
	return MyString_StrCompareICase(this->v, s->v);
}

template <typename T> OSInt Text::StringBase<T>::CompareToICase(const UTF8Char *s) const
{
	return MyString_StrCompareICase(this->v, s);
}

template <typename T> OSInt Text::StringBase<T>::CompareToFast(StringBase<const UTF8Char> s) const
{
	const UTF8Char *s0 = this->v;
	UOSInt len1 = this->leng;
	UOSInt len2 = s.leng;
	const UTF8Char *str2 = s.v;
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

template <typename T> Int32 Text::StringBase<T>::ToInt32() const
{
	return Text::StrToInt32(this->v);
}

template <typename T> UInt32 Text::StringBase<T>::ToUInt32() const
{
	return Text::StrToUInt32(this->v);
}

template <typename T> Int64 Text::StringBase<T>::ToInt64() const
{
	return Text::StrToInt64(this->v);
}

template <typename T> UInt64 Text::StringBase<T>::ToUInt64() const
{
	return Text::StrToUInt64(this->v);
}

template <typename T> OSInt Text::StringBase<T>::ToOSInt() const
{
	return Text::StrToOSInt(this->v);
}

template <typename T> UOSInt Text::StringBase<T>::ToUOSInt() const
{
	return Text::StrToUOSInt(this->v);
}

template <typename T> Double Text::StringBase<T>::ToDouble() const
{
	return Text::StrToDouble(this->v);
}

template <typename T> Bool Text::StringBase<T>::ToUInt8(UInt8 *outVal) const
{
	return Text::StrToUInt8(this->v, outVal);
}

template <typename T> Bool Text::StringBase<T>::ToInt16(Int16 *outVal) const
{
	return Text::StrToInt16(this->v, outVal);
}

template <typename T> Bool Text::StringBase<T>::ToUInt16(UInt16 *outVal) const
{
	return Text::StrToUInt16(this->v, outVal);
}

template <typename T> Bool Text::StringBase<T>::ToInt32(Int32 *outVal) const
{
	return Text::StrToInt32(this->v, outVal);
}

template <typename T> Bool Text::StringBase<T>::ToUInt32(UInt32 *outVal) const
{
	return Text::StrToUInt32(this->v, outVal);
}

template <typename T> Bool Text::StringBase<T>::ToInt64(Int64 *outVal) const
{
	return Text::StrToInt64(this->v, outVal);
}

template <typename T> Bool Text::StringBase<T>::ToUInt64(UInt64 *outVal) const
{
	return Text::StrToUInt64(this->v, outVal);
}

template <typename T> Bool Text::StringBase<T>::ToOSInt(OSInt *outVal) const
{
	return Text::StrToOSInt(this->v, outVal);
}

template <typename T> Bool Text::StringBase<T>::ToUOSInt(UOSInt *outVal) const
{
	return Text::StrToUOSInt(this->v, outVal);
}

template <typename T> Bool Text::StringBase<T>::ToDouble(Double *outVal) const
{
	return Text::StrToDouble(this->v, outVal);
}

template <typename T> Bool Text::StringBase<T>::ToUInt16S(UInt16 *outVal, UInt16 failVal) const
{
	return Text::StrToUInt16S(this->v, outVal, failVal);
}

template <typename T> Bool Text::StringBase<T>::ToUInt32S(UInt32 *outVal, UInt32 failVal) const
{
	return Text::StrToUInt32S(this->v, outVal, failVal);
}

template<typename T> UOSInt Text::StringBase<T>::Hex2Bytes(UInt8 *buff) const
{
	return Text::StrHex2Bytes(this->v, buff);
}

template<typename T> UOSInt Text::StringBase<T>::CountChar(UTF8Char c) const
{
	return Text::StrCountChar(this->v, c);
}

template<typename T> UOSInt Text::StringBase<T>::CountStr(const UTF8Char *s, UOSInt len) const
{
	return Text::StrCountStr(this->v, this->leng, s, len);
}

template<typename T> UOSInt Text::StringBase<T>::CountStr(StringBase<UTF8Char> *s) const
{
	return Text::StrCountStr(this->v, this->leng, s->v, s->leng);
}

template<typename T> UOSInt Text::StringBase<T>::CountStr(const StringBase<const UTF8Char> &s) const
{
	return Text::StrCountStr(this->v, this->leng, s.v, s.leng);
}

template<typename T> Bool Text::StringBase<T>::Hex2UInt16(UInt16 *outVal) const
{
	if (this->leng > 4)
	{
		return false;
	}
	else
	{
		return Text::StrHex2UInt16V(this->v, outVal);
	}
}

template<typename T> Bool Text::StringBase<T>::Hex2UInt32(UInt32 *outVal) const
{
	if (this->leng > 8)
	{
		return false;
	}
	else
	{
		return Text::StrHex2UInt32V(this->v, outVal);
	}
}

template <typename T> Double Text::StringBase<T>::MatchRating(StringBase<UTF8Char> *s) const
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

template <typename T> Double Text::StringBase<T>::MatchRating(const UTF8Char *targetStr, UOSInt strLen) const
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

template <typename T> Bool Text::StringBase<T>::operator==(StringBase<T> s) const
{
	return Text::StrEqualsC(this->v, this->leng, s.v, s.leng);
}

#endif
