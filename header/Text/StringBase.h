#ifndef _SM_TEXT_STRINGBASE
#define _SM_TEXT_STRINGBASE
#include "Data/ByteTool.h"
#include "Data/ByteArray.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

namespace Text
{
	template <typename T> struct StringBase
	{
		UnsafeArray<T> v;
		UOSInt leng;

		T *GetEndPtr();
		Data::ByteArray ToByteArray() const;

		UnsafeArray<UTF8Char> ConcatTo(UnsafeArray<UTF8Char> sbuff) const;
		UnsafeArray<UTF8Char> ConcatWith(UnsafeArray<UTF8Char> sbuff, UnsafeArray<const UTF8Char> s1, UOSInt len1) const;
		UnsafeArray<UTF8Char> ConcatToS(UnsafeArray<UTF8Char> sbuff, UOSInt buffSize) const;
		Bool Equals(NN<StringBase<UTF8Char>> s) const;
		Bool Equals(StringBase<const UTF8Char> s) const;
		Bool Equals(UnsafeArray<const UTF8Char> s, UOSInt len) const;
		Bool EqualsICase(NN<StringBase<UTF8Char>> s) const;
		Bool EqualsICase(StringBase<const UTF8Char> s) const;
		Bool EqualsICase(UnsafeArray<const UTF8Char> s, UOSInt len) const;
		Bool StartsWith(UTF8Char c) const;
		Bool StartsWith(NN<StringBase<UTF8Char>> s) const;
		Bool StartsWith(StringBase<const UTF8Char> s) const;
		Bool StartsWith(UnsafeArray<const UTF8Char> s, UOSInt len) const;
		Bool StartsWith(UOSInt startIndex, UnsafeArray<const UTF8Char> s, UOSInt len) const;
		Bool StartsWithICase(UnsafeArray<const UTF8Char> s, UOSInt len) const;
		Bool StartsWithICase(UOSInt startIndex, UnsafeArray<const UTF8Char> s, UOSInt len) const;
		Bool EndsWith(UTF8Char c) const;
		Bool EndsWith(UnsafeArray<const UTF8Char> s, UOSInt len) const;
		Bool EndsWith(StringBase<const UTF8Char> s) const;
		Bool EndsWithICase(UnsafeArray<const UTF8Char> s, UOSInt len) const;
		Bool HasUpperCase() const;
		Bool ContainChars(UnsafeArray<const UTF8Char> chars) const;
		UOSInt IndexOf(UnsafeArray<const UTF8Char> s, UOSInt len) const;
		UOSInt IndexOf(NN<StringBase<UTF8Char>> s) const;
		UOSInt IndexOf(StringBase<const UTF8Char> s) const;
		UOSInt IndexOf(UnsafeArray<const UTF8Char> s, UOSInt len, UOSInt startIndex) const;
		UOSInt IndexOf(UTF8Char c) const;
		UOSInt IndexOf(UTF8Char c, UOSInt startIndex) const;
		UOSInt IndexOfICase(UnsafeArray<const UTF8Char> s, UOSInt len) const;
		UOSInt IndexOfICase(StringBase<const UTF8Char> s) const;
		UOSInt IndexOfICase(NN<StringBase<UTF8Char>> s) const;
		UOSInt LastIndexOf(UTF8Char c) const;
		UOSInt LastIndexOf(UTF8Char c, UOSInt startIndex) const;
		OSInt CompareTo(NN<StringBase<UTF8Char>> s) const;
		OSInt CompareTo(StringBase<const UTF8Char> s) const;
		OSInt CompareTo(UnsafeArray<const UTF8Char> s) const;
		OSInt CompareToICase(NN<StringBase<UTF8Char>> s) const;
		OSInt CompareToICase(UnsafeArray<const UTF8Char> s) const;
		OSInt CompareToFast(StringBase<const UTF8Char> s) const;

		Int32 ToInt32() const;
		NInt32 ToNInt32() const;
		UInt32 ToUInt32() const;
		Int64 ToInt64() const;
		UInt64 ToUInt64() const;
		OSInt ToOSInt() const;
		UOSInt ToUOSInt() const;
		Double ToDouble() const;
		Bool ToUInt8(OutParam<UInt8> outVal) const;
		Bool ToInt16(OutParam<Int16> outVal) const;
		Bool ToUInt16(OutParam<UInt16> outVal) const;
		Bool ToInt32(OutParam<Int32> outVal) const;
		Bool ToUInt32(OutParam<UInt32> outVal) const;
		Bool ToInt64(OutParam<Int64> outVal) const;
		Bool ToUInt64(OutParam<UInt64> outVal) const;
		Bool ToOSInt(OutParam<OSInt> outVal) const;
		Bool ToUOSInt(OutParam<UOSInt> outVal) const;
		Bool ToDouble(OutParam<Double> outVal) const;
		Bool ToUInt16S(OutParam<UInt16> outVal, UInt16 failVal) const;
		Bool ToUInt32S(OutParam<UInt32> outVal, UInt32 failVal) const;
		Bool ToBool() const;
		UOSInt Hex2Bytes(UInt8 *buff) const;
		UOSInt CountChar(UTF8Char c) const;
		UOSInt CountStr(UnsafeArray<const UTF8Char> s, UOSInt len) const;
		UOSInt CountStr(NN<StringBase<UTF8Char>> s) const;
		UOSInt CountStr(const StringBase<const UTF8Char> &s) const;
		Bool Hex2UInt16(OutParam<UInt16> outVal) const;
		Bool Hex2UInt32(OutParam<UInt32> outVal) const;

		Double MatchRating(NN<StringBase<UTF8Char>> s) const;
		Double MatchRating(UnsafeArray<const UTF8Char> targetStr, UOSInt strLen) const;
		UOSInt BranketSearch(UOSInt startIndex, UTF8Char c) const;
		Bool HasAlphaNumeric() const;

		Bool operator==(StringBase<T> s) const;
	};
}

template <typename T> T *Text::StringBase<T>::GetEndPtr()
{
	return &this->v[this->leng];
}

template <typename T> Data::ByteArray Text::StringBase<T>::ToByteArray() const
{
	return Data::ByteArray(UnsafeArray<UInt8>::ConvertFrom(this->v), this->leng * sizeof(T));
}

template <typename T> UnsafeArray<UTF8Char> Text::StringBase<T>::ConcatTo(UnsafeArray<UTF8Char> sbuff) const
{
	REGVAR UOSInt len = this->leng;
	MemCopyNO(sbuff.Ptr(), this->v.Ptr(), len);
	sbuff += len;
	sbuff[0] = 0;
	return sbuff;
}

template <typename T> UnsafeArray<UTF8Char> Text::StringBase<T>::ConcatWith(UnsafeArray<UTF8Char> sbuff, UnsafeArray<const UTF8Char> s1, UOSInt len1) const
{
	REGVAR UOSInt len = this->leng;
	MemCopyNO(sbuff.Ptr(), this->v.Ptr(), len);
	sbuff += len;
	MemCopyNO(sbuff.Ptr(), s1.Ptr(), len1);
	sbuff += len1;
	sbuff[0] = 0;
	return sbuff;
}

template <typename T> UnsafeArray<UTF8Char> Text::StringBase<T>::ConcatToS(UnsafeArray<UTF8Char> sbuff, UOSInt buffSize) const
{
	if (buffSize > this->leng)
	{
		MemCopyNO(sbuff.Ptr(), this->v.Ptr(), this->leng);
		sbuff += this->leng;
		sbuff[0] = 0;
		return sbuff;
	}
	else
	{
		buffSize--;
		MemCopyNO(sbuff.Ptr(), this->v.Ptr(), buffSize);
		sbuff[buffSize] = 0;
		return &sbuff[buffSize];
	}
}

template <typename T> Bool Text::StringBase<T>::Equals(NN<StringBase<UTF8Char>> s) const
{
	return Equals(s->v, s->leng);
}

template <typename T> Bool Text::StringBase<T>::Equals(StringBase<const UTF8Char> s) const
{
	return Equals(s.v, s.leng);
}

template <typename T> Bool Text::StringBase<T>::Equals(UnsafeArray<const UTF8Char> s, UOSInt len) const
{
	return Text::StrEqualsC(this->v, this->leng, s, len);
}

template <typename T> Bool Text::StringBase<T>::EqualsICase(NN<StringBase<UTF8Char>> s) const
{
	return EqualsICase(s->v, s->leng);
}

template <typename T> Bool Text::StringBase<T>::EqualsICase(StringBase<const UTF8Char> s) const
{
	return EqualsICase(s.v, s.leng);
}

template <typename T> Bool Text::StringBase<T>::EqualsICase(UnsafeArray<const UTF8Char> s, UOSInt len) const
{
	return Text::StrEqualsICaseC(this->v, this->leng, s, len);
}

template <typename T> Bool Text::StringBase<T>::StartsWith(UTF8Char c) const
{
	return this->v[0] == c;
}

template <typename T> Bool Text::StringBase<T>::StartsWith(NN<StringBase<UTF8Char>> s) const
{
	return StartsWith(s->v, s->leng);
}

template <typename T> Bool Text::StringBase<T>::StartsWith(StringBase<const UTF8Char> s) const
{
	return StartsWith(s.v, s.leng);
}

template <typename T> Bool Text::StringBase<T>::StartsWith(UnsafeArray<const UTF8Char> s, UOSInt len) const
{
	return Text::StrStartsWithC(this->v, this->leng, s, len);
}

template <typename T> Bool Text::StringBase<T>::StartsWith(UOSInt startIndex, UnsafeArray<const UTF8Char> s, UOSInt len) const
{
	if (startIndex > this->leng)
	{
		return false;
	}
	return Text::StrStartsWithC(&this->v[startIndex], this->leng - startIndex, s, len);
}

template <typename T> Bool Text::StringBase<T>::StartsWithICase(UnsafeArray<const UTF8Char> str2, UOSInt len) const
{
	return Text::StrStartsWithICaseC(this->v, this->leng, str2, len);
}

template <typename T> Bool Text::StringBase<T>::StartsWithICase(UOSInt startIndex, UnsafeArray<const UTF8Char> s, UOSInt len) const
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

template <typename T> Bool Text::StringBase<T>::EndsWith(UnsafeArray<const UTF8Char> s, UOSInt len2) const
{
	REGVAR UOSInt len1 = this->leng;
	if (len2 > len1)
	{
		return false;
	}
	return Text::StrEqualsC(&this->v[len1 - len2], len2, s, len2);
}

template <typename T> Bool Text::StringBase<T>::EndsWith(StringBase<const UTF8Char> s) const
{
	return EndsWith(s.v, s.leng);
}

template <typename T> Bool Text::StringBase<T>::EndsWithICase(UnsafeArray<const UTF8Char> s, UOSInt len2) const
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

template <typename T> Bool Text::StringBase<T>::ContainChars(UnsafeArray<const UTF8Char> chars) const
{
	return Text::StrContainChars(this->v, chars);
}

template <typename T> UOSInt Text::StringBase<T>::IndexOf(UnsafeArray<const UTF8Char> s, UOSInt len) const
{
	return Text::StrIndexOfC(this->v, this->leng, s, len);
}

template <typename T> UOSInt Text::StringBase<T>::IndexOf(NN<StringBase<UTF8Char>> s) const
{
	return IndexOf(s->v, s->leng);
}

template <typename T> UOSInt Text::StringBase<T>::IndexOf(StringBase<const UTF8Char> s) const
{
	return IndexOf(s.v, s.leng);
}

template <typename T> UOSInt Text::StringBase<T>::IndexOf(UnsafeArray<const UTF8Char> s, UOSInt len, UOSInt startIndex) const
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

template <typename T> UOSInt Text::StringBase<T>::IndexOfICase(UnsafeArray<const UTF8Char> s, UOSInt len) const
{
	return Text::StrIndexOfICase(this->v, s);
}

template <typename T> UOSInt Text::StringBase<T>::IndexOfICase(StringBase<const UTF8Char> s) const
{
	return IndexOfICase(s.v, s.leng);
}

template <typename T> UOSInt Text::StringBase<T>::IndexOfICase(NN<StringBase<UTF8Char>> s) const
{
	return IndexOfICase(s->v, s->leng);
}

template <typename T> UOSInt Text::StringBase<T>::LastIndexOf(UTF8Char c) const
{
	return Text::StrLastIndexOfCharC(this->v, this->leng, c);
}

template <typename T> UOSInt Text::StringBase<T>::LastIndexOf(UTF8Char c, UOSInt startIndex) const
{
	if (startIndex >= this->leng)
		return INVALID_INDEX;
	UOSInt i = Text::StrLastIndexOfCharC(this->v + startIndex, this->leng - startIndex, c);
	if (i == INVALID_INDEX)
		return INVALID_INDEX;
	return i + startIndex;
}

template <typename T> OSInt Text::StringBase<T>::CompareTo(NN<StringBase<UTF8Char>> s) const
{
	return MyString_StrCompare(this->v.Ptr(), s->v.Ptr());
}

template <typename T> OSInt Text::StringBase<T>::CompareTo(StringBase<const UTF8Char> s) const
{
	return MyString_StrCompare(this->v.Ptr(), s.v.Ptr());
}

template <typename T> OSInt Text::StringBase<T>::CompareTo(UnsafeArray<const UTF8Char> s) const
{
	return MyString_StrCompare(this->v.Ptr(), s.Ptr());
}

template <typename T> OSInt Text::StringBase<T>::CompareToICase(NN<StringBase<UTF8Char>> s) const
{
	return MyString_StrCompareICase(this->v.Ptr(), s->v.Ptr());
}

template <typename T> OSInt Text::StringBase<T>::CompareToICase(UnsafeArray<const UTF8Char> s) const
{
	return MyString_StrCompareICase(this->v.Ptr(), s.Ptr());
}

template <typename T> OSInt Text::StringBase<T>::CompareToFast(StringBase<const UTF8Char> s) const
{
	UnsafeArray<const UTF8Char> s0 = this->v;
	UOSInt len1 = this->leng;
	UOSInt len2 = s.leng;
	UnsafeArray<const UTF8Char> str2 = s.v;
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
		REGVAR UInt32 v1 = ReadMUInt32(&s0[0]);
		REGVAR UInt32 v2 = ReadMUInt32(&str2[0]);
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

template <typename T> NInt32 Text::StringBase<T>::ToNInt32() const
{
	Int32 v;
	if (Text::StrToInt32(this->v, v))
		return v;
	else
		return nullptr;
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

template <typename T> Bool Text::StringBase<T>::ToUInt8(OutParam<UInt8> outVal) const
{
	return Text::StrToUInt8(this->v, outVal);
}

template <typename T> Bool Text::StringBase<T>::ToInt16(OutParam<Int16> outVal) const
{
	return Text::StrToInt16(this->v, outVal);
}

template <typename T> Bool Text::StringBase<T>::ToUInt16(OutParam<UInt16> outVal) const
{
	return Text::StrToUInt16(this->v, outVal);
}

template <typename T> Bool Text::StringBase<T>::ToInt32(OutParam<Int32> outVal) const
{
	return Text::StrToInt32(this->v, outVal);
}

template <typename T> Bool Text::StringBase<T>::ToUInt32(OutParam<UInt32> outVal) const
{
	return Text::StrToUInt32(this->v, outVal);
}

template <typename T> Bool Text::StringBase<T>::ToInt64(OutParam<Int64> outVal) const
{
	return Text::StrToInt64(this->v, outVal);
}

template <typename T> Bool Text::StringBase<T>::ToUInt64(OutParam<UInt64> outVal) const
{
	return Text::StrToUInt64(this->v, outVal);
}

template <typename T> Bool Text::StringBase<T>::ToOSInt(OutParam<OSInt> outVal) const
{
	return Text::StrToOSInt(this->v, outVal);
}

template <typename T> Bool Text::StringBase<T>::ToUOSInt(OutParam<UOSInt> outVal) const
{
	return Text::StrToUOSInt(this->v, outVal);
}

template <typename T> Bool Text::StringBase<T>::ToDouble(OutParam<Double> outVal) const
{
	return Text::StrToDouble(this->v, outVal);
}

template <typename T> Bool Text::StringBase<T>::ToUInt16S(OutParam<UInt16> outVal, UInt16 failVal) const
{
	return Text::StrToUInt16S(this->v, outVal, failVal);
}

template <typename T> Bool Text::StringBase<T>::ToUInt32S(OutParam<UInt32> outVal, UInt32 failVal) const
{
	return Text::StrToUInt32S(this->v, outVal, failVal);
}

template <typename T> Bool Text::StringBase<T>::ToBool() const
{
	return Text::StrToBool(UnsafeArray<const UTF8Char>(this->v));
}

template<typename T> UOSInt Text::StringBase<T>::Hex2Bytes(UInt8 *buff) const
{
	return Text::StrHex2Bytes(this->v, buff);
}

template<typename T> UOSInt Text::StringBase<T>::CountChar(UTF8Char c) const
{
	return Text::StrCountChar(this->v, c);
}

template<typename T> UOSInt Text::StringBase<T>::CountStr(UnsafeArray<const UTF8Char> s, UOSInt len) const
{
	return Text::StrCountStr(this->v, this->leng, s, len);
}

template<typename T> UOSInt Text::StringBase<T>::CountStr(NN<StringBase<UTF8Char>> s) const
{
	return Text::StrCountStr(this->v, this->leng, s->v, s->leng);
}

template<typename T> UOSInt Text::StringBase<T>::CountStr(const StringBase<const UTF8Char> &s) const
{
	return Text::StrCountStr(this->v, this->leng, s.v, s.leng);
}

template<typename T> Bool Text::StringBase<T>::Hex2UInt16(OutParam<UInt16> outVal) const
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

template<typename T> Bool Text::StringBase<T>::Hex2UInt32(OutParam<UInt32> outVal) const
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

template <typename T> Double Text::StringBase<T>::MatchRating(NN<StringBase<UTF8Char>> s) const
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

template <typename T> Double Text::StringBase<T>::MatchRating(UnsafeArray<const UTF8Char> targetStr, UOSInt strLen) const
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

template <typename T> UOSInt Text::StringBase<T>::BranketSearch(UOSInt startIndex, UTF8Char c) const
{
	if (leng <= startIndex)
		return INVALID_INDEX;
	UTF8Char branketList[20];
	UOSInt branketCnt = 0;
	UTF8Char thisC;
	UnsafeArray<const T> curr = this->v + startIndex;
	UnsafeArray<const T> endPtr = this->v + this->leng;
	while (curr < endPtr)
	{
		thisC = *curr++;
		if (thisC == c)
		{
			if (branketCnt == 0)
				return (UOSInt)(curr - this->v - 1);
			branketCnt--;
			c = branketList[branketCnt];
		}
		else
		{
			switch (thisC)
			{
			case '\'':
			case '\"':
			case '`':
				while (curr < endPtr && *curr++ != thisC);
				break;
			case '[':
				if (branketCnt == 20)
					return INVALID_INDEX;
				branketList[branketCnt++] = c;
				c = ']';
				break;
			case '(':
				if (branketCnt == 20)
					return INVALID_INDEX;
				branketList[branketCnt++] = c;
				c = ')';
				break;
			case '{':
				if (branketCnt == 20)
					return INVALID_INDEX;
				branketList[branketCnt++] = c;
				c = '}';
				break;
			}
		}
	}
	return INVALID_INDEX;
}

template <typename T> Bool Text::StringBase<T>::HasAlphaNumeric() const
{
	UOSInt i = this->leng;
	UnsafeArray<UTF8Char> ptr = this->v;
	UTF8Char c;
	while (i-- > 0)
	{
		c = *ptr++;
		if (c >= '0' && c <= '9')
			return true;
		if (c >= 'A' && c <= 'Z')
			return true;
		if (c >= 'a' && c <= 'z')
			return true;
	}
	return false;
}

template <typename T> Bool Text::StringBase<T>::operator==(StringBase<T> s) const
{
	return Text::StrEqualsC(this->v, this->leng, s.v, s.leng);
}

#endif
