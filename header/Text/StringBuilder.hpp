#ifndef _SM_TEXT_STRINGBUILDER
#define _SM_TEXT_STRINGBUILDER
#include "MemTool.h"
#include "MyMemory.h"
#include "Data/DateTime.h"
#include "Text/LineBreakType.h"
#include "Text/MyString.h"

namespace Text
{
	template <class T> class StringBuilder
	{
	protected:
		UnsafeArray<T> buff;
		UnsafeArray<T> buffEnd;
		UOSInt buffSize;
	public:
		StringBuilder();
		virtual ~StringBuilder();

		NN<StringBuilder<T>> AppendI16(Int16 iVal);
		NN<StringBuilder<T>> AppendU16(UInt16 iVal);
		NN<StringBuilder<T>> AppendI32(Int32 iVal);
		NN<StringBuilder<T>> AppendU32(UInt32 iVal);
		NN<StringBuilder<T>> AppendI64(Int64 iVal);
		NN<StringBuilder<T>> AppendU64(UInt64 iVal);
		NN<StringBuilder<T>> AppendDate(NN<Data::DateTime> dt);
		NN<StringBuilder<T>> AppendOSInt(OSInt iVal);
		NN<StringBuilder<T>> AppendUOSInt(UOSInt iVal);
		NN<StringBuilder<T>> AppendSB(NN<Text::StringBuilder<T>> sb);

		NN<StringBuilder<T>> AppendHex8(UInt8 iVal);
		NN<StringBuilder<T>> AppendHex16(UInt16 iVal);
		NN<StringBuilder<T>> AppendHex24(UInt32 iVal);
		NN<StringBuilder<T>> AppendHex32(UInt32 iVal);
		NN<StringBuilder<T>> AppendHex32V(UInt32 iVal);
		NN<StringBuilder<T>> AppendHex64(UInt64 iVal);
		NN<StringBuilder<T>> AppendHex64V(UInt64 iVal);
		NN<StringBuilder<T>> AppendHexOS(UOSInt iVal);
		NN<StringBuilder<T>> AppendHex(UnsafeArray<const UInt8> buff, UOSInt buffSize, T seperator, LineBreakType lineBreak);

		void ClearStr();
		void AllocLeng(UOSInt leng);
		UOSInt GetLength();
		void RemoveChars(UOSInt cnt);
		void RemoveChars(UOSInt index, UOSInt cnt);
		void Trim();
		void TrimWSCRLF();
		void TrimRight();
		void TrimToLength(UOSInt leng);
		NN<StringBuilder<T>> SetSubstr(UOSInt index);
		void ToUpper();
		void ToLower();
		void ToCapital();

		Bool ToUInt16(OutParam<UInt16> outVal);
		Bool ToUInt32(OutParam<UInt32> outVal);
		Bool ToUInt32S(OutParam<UInt32> outVal, UInt32 failVal);
		Bool ToUOSInt(OutParam<UOSInt> outVal);
		Bool ToInt32(OutParam<Int32> outVal);
		Int32 ToInt32();
		Bool ToInt64(OutParam<Int64> outVal);
		Int64 ToInt64();
		Bool ToUInt64(OutParam<UInt64> outVal);
		UInt64 ToUInt64();
		Bool ToOSInt(OutParam<OSInt> outVal);
		UOSInt Hex2Bytes(OutParam<UInt8> buff);

		UnsafeArray<T> ToString();
		T *ToPtr();
		UnsafeArray<T> GetEndPtr();
		void SetEndPtr(UnsafeArray<T> ptr);
		UOSInt IndexOf(UnsafeArray<const T> s);
		UOSInt IndexOf(UnsafeArray<const T> s, UOSInt index);
		UOSInt IndexOf(T c);
		UOSInt LastIndexOf(T c);
		Bool Equals(UnsafeArray<const T> s);
		Bool EqualsICase(UnsafeArray<const T> s);
		Bool StartsWith(UnsafeArray<const T> s);
		Bool EndsWith(T c);
		UnsafeArray<T> SubString(UnsafeArray<T> buff, UOSInt start, UOSInt length);
		UOSInt Replace(T fromChar, T toChar);
		UOSInt Replace(UnsafeArray<const T> fromStr, UnsafeArray<const T> toStr);
		UOSInt ReplaceICase(UnsafeArray<const T> fromStr, UnsafeArray<const T> toStr);
	};

	template <class T> Text::StringBuilder<T>::StringBuilder()
	{
		this->buff = MemAllocArr(T, this->buffSize = 1024);
		this->buffEnd = this->buff;
		this->buff[0] = 0;
	}

	template<class T> Text::StringBuilder<T>::~StringBuilder()
	{
		MemFreeArr(this->buff);
	}

	template <class T> NN<Text::StringBuilder<T>> Text::StringBuilder<T>::AppendI16(Int16 iVal)
	{
		this->AllocLeng(6);
		this->buffEnd = Text::StrInt16(this->buffEnd.Ptr(), iVal);
		return NNTHIS;
	}

	template <class T> NN<Text::StringBuilder<T>> Text::StringBuilder<T>::AppendU16(UInt16 iVal)
	{
		this->AllocLeng(5);
		this->buffEnd = Text::StrUInt16(this->buffEnd.Ptr(), iVal);
		return NNTHIS;
	}

	template <class T> NN<Text::StringBuilder<T>> Text::StringBuilder<T>::AppendI32(Int32 iVal)
	{
		this->AllocLeng(11);
		this->buffEnd = Text::StrInt32(this->buffEnd.Ptr(), iVal);
		return NNTHIS;
	}

	template <class T> NN<Text::StringBuilder<T>> Text::StringBuilder<T>::AppendU32(UInt32 iVal)
	{
		this->AllocLeng(10);
		this->buffEnd = Text::StrUInt32(this->buffEnd.Ptr(), iVal);
		return NNTHIS;
	}

	template <class T> NN<Text::StringBuilder<T>> Text::StringBuilder<T>::AppendI64(Int64 iVal)
	{
		this->AllocLeng(22);
		this->buffEnd = Text::StrInt64(this->buffEnd.Ptr(), iVal);
		return NNTHIS;
	}

	template <class T> NN<Text::StringBuilder<T>> Text::StringBuilder<T>::AppendU64(UInt64 iVal)
	{
		this->AllocLeng(20);
		this->buffEnd = Text::StrUInt64(this->buffEnd.Ptr(), iVal);
		return NNTHIS;
	}

	template <class T> NN<Text::StringBuilder<T>> Text::StringBuilder<T>::AppendDate(NN<Data::DateTime> dt)
	{
		UTF8Char sbuff[30];
		UnsafeArray<UTF8Char> sptr;
		sptr = dt->ToString(sbuff, CHSTR("yyyy-MM-dd HH:mm:ss"));
		this->AllocLeng(19);
		this->buffEnd = Text::StrConcatC(this->buffEnd.Ptr(), (const T*)sbuff, (UOSInt)(sptr - sbuff));
		return NNTHIS;
	}

	template <class T> NN<Text::StringBuilder<T>> Text::StringBuilder<T>::AppendOSInt(OSInt iVal)
	{
	#if _OSINT_SIZE == 64
		this->AllocLeng(22);
		this->buffEnd = Text::StrInt64(this->buffEnd.Ptr(), iVal);
	#else
		this->AllocLeng(11);
		this->buffEnd = Text::StrInt32(this->buffEnd.Ptr(), (Int32)iVal);
	#endif
		return NNTHIS;
	}

	template <class T> NN<Text::StringBuilder<T>> Text::StringBuilder<T>::AppendUOSInt(UOSInt iVal)
	{
	#if _OSINT_SIZE == 64
		this->AllocLeng(22);
		this->buffEnd = Text::StrUInt64(this->buffEnd.Ptr(), iVal);
	#else
		this->AllocLeng(11);
		this->buffEnd = Text::StrUInt32(this->buffEnd.Ptr(), (UInt32)iVal);
	#endif
		return NNTHIS;
	}

	template <class T> NN<Text::StringBuilder<T>> Text::StringBuilder<T>::AppendSB(NN<Text::StringBuilder<T>> sb)
	{
		UOSInt slen = (UOSInt)(sb->buffEnd - sb->buff);
		this->AllocLeng(slen);
		MemCopyNO(this->buffEnd.Ptr(), sb->buff, (slen + 1) * sizeof(T));
		this->buffEnd = &this->buffEnd[slen];
		return NNTHIS;
	}

	template <class T> NN<Text::StringBuilder<T>> Text::StringBuilder<T>::AppendHex8(UInt8 iVal)
	{
		this->AllocLeng(2);
		this->buffEnd = Text::StrHexByte(this->buffEnd.Ptr(), iVal);
		return NNTHIS;
	}

	template <class T> NN<Text::StringBuilder<T>> Text::StringBuilder<T>::AppendHex16(UInt16 iVal)
	{
		this->AllocLeng(4);
		this->buffEnd = Text::StrHexVal16(this->buffEnd.Ptr(), iVal);
		return NNTHIS;
	}

	template <class T> NN<Text::StringBuilder<T>> Text::StringBuilder<T>::AppendHex24(UInt32 iVal)
	{
		this->AllocLeng(6);
		this->buffEnd = Text::StrHexVal24(this->buffEnd.Ptr(), iVal);
		return NNTHIS;
	}

	template <class T> NN<Text::StringBuilder<T>> Text::StringBuilder<T>::AppendHex32(UInt32 iVal)
	{
		this->AllocLeng(8);
		this->buffEnd = Text::StrHexVal32(this->buffEnd.Ptr(), iVal);
		return NNTHIS;
	}

	template <class T> NN<Text::StringBuilder<T>> Text::StringBuilder<T>::AppendHex32V(UInt32 iVal)
	{
		this->AllocLeng(8);
		this->buffEnd = Text::StrHexVal32V(this->buffEnd.Ptr(), iVal);
		return NNTHIS;
	}

	template <class T> NN<Text::StringBuilder<T>> Text::StringBuilder<T>::AppendHex64(UInt64 iVal)
	{
		this->AllocLeng(16);
		this->buffEnd = Text::StrHexVal64(this->buffEnd.Ptr(), iVal);
		return NNTHIS;
	}

	template <class T> NN<Text::StringBuilder<T>> Text::StringBuilder<T>::AppendHex64V(UInt64 iVal)
	{
		this->AllocLeng(16);
		this->buffEnd = Text::StrHexVal64V(this->buffEnd.Ptr(), iVal);
		return NNTHIS;
	}

	template <class T> NN<Text::StringBuilder<T>> Text::StringBuilder<T>::AppendHexOS(UOSInt iVal)
	{
	#if _OSINT_SIZE == 64
		return AppendHex64(iVal);
	#elif _OSINT_SIZE == 32
		return AppendHex32((UInt32)iVal);
	#elif _OSINT_SIZE == 16
		return AppendHex16((Int32)iVal);
	#endif
	}

	template <class T> NN<Text::StringBuilder<T>> Text::StringBuilder<T>::AppendHex(UnsafeArray<const UInt8> buff, UOSInt buffSize, T seperator, LineBreakType lineBreak)
	{
		UOSInt lbCnt = (buffSize >> 4);
		OSInt i;
		if (buffSize == 0)
			return NNTHIS;
		if ((buffSize & 15) == 0)
			lbCnt -= 1;
		if (lineBreak == LineBreakType::CRLF)
			lbCnt = lbCnt << 2;
		else if (lineBreak == LineBreakType::CR || lineBreak == LineBreakType::LF)
		{
		}
		else
		{
			lbCnt = 0;
		}
		i = 0;
		if (seperator == 0)
		{
			this->AllocLeng((buffSize << 1) + lbCnt);
			while (buffSize-- > 0)
			{
				this->buffEnd[0] = (T)MyString_STRHEXARR[buff[0] >> 4];
				this->buffEnd[1] = (T)MyString_STRHEXARR[buff[0] & 15];
				this->buffEnd += 2;
				buff++;
				i++;
				if ((i & 15) == 0 && buffSize > 0)
				{
					if (lineBreak == LineBreakType::CRLF)
					{
						*buffEnd++ = '\r';
						*buffEnd++ = '\n';
					}
					else if (lineBreak == LineBreakType::CR)
					{
						*buffEnd++ = '\r';
					}
					else if (lineBreak == LineBreakType::LF)
					{
						*buffEnd++ = '\n';
					}
				}
			}
			this->buffEnd[0] = 0;
		}
		else
		{
			this->AllocLeng(buffSize * 3 + lbCnt - 1);
			while (buffSize-- > 0)
			{
				this->buffEnd[0] = (T)MyString_STRHEXARR[buff[0] >> 4];
				this->buffEnd[1] = (T)MyString_STRHEXARR[buff[0] & 15];
				this->buffEnd[2] = seperator;
				this->buffEnd += 3;
				buff++;
				i++;
				if ((i & 15) == 0 && buffSize > 0)
				{
					if (lineBreak == LineBreakType::CRLF)
					{
						*buffEnd++ = '\r';
						*buffEnd++ = '\n';
					}
					else if (lineBreak == LineBreakType::CR)
					{
						*buffEnd++ = '\r';
					}
					else if (lineBreak == LineBreakType::LF)
					{
						*buffEnd++ = '\n';
					}
				}
			}
			*--buffEnd = 0;
		}
		return NNTHIS;
	}

	template<class T> void Text::StringBuilder<T>::ClearStr()
	{
		this->buffEnd = this->buff;
		this->buff[0] = 0;
	}

	template<class T> void Text::StringBuilder<T>::AllocLeng(UOSInt leng)
	{
		UOSInt slen = leng * sizeof(T);
		UOSInt currSize = (UOSInt)(this->buffEnd - this->buff + 1) * sizeof(T);
		while (slen + currSize > this->buffSize)
		{
			this->buffSize <<= 1;
			UnsafeArray<T> newStr = MemAllocArr(T, this->buffSize);
			MemCopyNO(newStr.Ptr(), this->buff.Ptr(), currSize);
			this->buffEnd = &newStr[(currSize / sizeof(T)) - 1];
			MemFreeArr(this->buff);
			this->buff = newStr;
		}
	}

	template<class T> UOSInt Text::StringBuilder<T>::GetLength()
	{
		return (UOSInt)(this->buffEnd - this->buff);
	}

	template<class T> void Text::StringBuilder<T>::RemoveChars(UOSInt cnt)
	{
		if ((OSInt)cnt >= (buffEnd - buff))
		{
			buffEnd = buff;
		}
		else
		{
			buffEnd -= cnt;
		}
		buffEnd[0] = 0;
	}

	template<class T> void Text::StringBuilder<T>::RemoveChars(UOSInt index, UOSInt cnt)
	{
		UOSInt endOfst = index + cnt;
		if (endOfst >= (UOSInt)(buffEnd - buff))
		{
			this->TrimToLength(index); 
		}
		else
		{
			buffEnd = Text::StrConcat(&buff[index], &buff[endOfst]);
		}
	}

	template<class T> void Text::StringBuilder<T>::Trim()
	{
		this->buffEnd = Text::StrTrim(this->buff.Ptr());
	}

	template<class T> void Text::StringBuilder<T>::TrimWSCRLF()
	{
		this->buffEnd = Text::StrTrimWSCRLF(this->buff);
	}

	template<class T> void Text::StringBuilder<T>::TrimRight()
	{
		this->buffEnd = Text::StrRTrim(this->buff);
	}

	template<class T> void Text::StringBuilder<T>::TrimToLength(UOSInt leng)
	{
		if ((this->buffEnd - this->buff) > (OSInt)leng)
		{
			this->buffEnd = &this->buff[leng];
			this->buffEnd[0] = 0;
		}
	}

	template<class T> NN<Text::StringBuilder<T>> Text::StringBuilder<T>::SetSubstr(UOSInt index)
	{
		if (index >= (UOSInt)(this->buffEnd - this->buff))
		{
			buffEnd = buff;
			buff[0] = 0;
		}
		else if (index > 0)
		{
			MemCopyO(this->buff, &this->buff[index], ((UOSInt)(this->buffEnd - this->buff) - index + 1) * sizeof(T));
			this->buffEnd -= index;
		}
		return NNTHIS;
	}

	template<class T> void Text::StringBuilder<T>::ToUpper()
	{
		Text::StrToUpper(this->buff, this->buff);
	}

	template<class T> void Text::StringBuilder<T>::ToLower()
	{
		Text::StrToLower(this->buff, this->buff);
	}

	template<class T> void Text::StringBuilder<T>::ToCapital()
	{
		Text::StrToCapital(this->buff, this->buff);
	}

	template<class T> Bool Text::StringBuilder<T>::ToUInt16(OutParam<UInt16> outVal)
	{
		return Text::StrToUInt16(this->buff, outVal);
	}

	template<class T> Bool Text::StringBuilder<T>::ToUInt32(OutParam<UInt32> outVal)
	{
		return Text::StrToUInt32(this->buff, outVal);
	}

	template<class T> Bool Text::StringBuilder<T>::ToUInt32S(OutParam<UInt32> outVal, UInt32 failVal)
	{
		return Text::StrToUInt32S(this->buff, outVal, failVal);
	}

	template<class T> Bool Text::StringBuilder<T>::ToUOSInt(OutParam<UOSInt> outVal)
	{
		return Text::StrToUOSInt(this->buff, outVal);
	}

	template<class T> Bool Text::StringBuilder<T>::ToInt32(OutParam<Int32> outVal)
	{
		return Text::StrToInt32(this->buff, outVal);
	}

	template<class T> Bool Text::StringBuilder<T>::ToOSInt(OutParam<OSInt> outVal)
	{
		return Text::StrToOSInt(this->buff, outVal);
	}

	template<class T> Int32 Text::StringBuilder<T>::ToInt32()
	{
		return Text::StrToInt32(this->buff);
	}

	template<class T> Bool Text::StringBuilder<T>::ToInt64(OutParam<Int64> outVal)
	{
		return Text::StrToInt64(this->buff, outVal);
	}

	template<class T> Int64 Text::StringBuilder<T>::ToInt64()
	{
		return Text::StrToInt64(this->buff);
	}

	template<class T> Bool Text::StringBuilder<T>::ToUInt64(OutParam<UInt64> outVal)
	{
		return Text::StrToUInt64(this->buff, outVal);
	}

	template<class T> UInt64 Text::StringBuilder<T>::ToUInt64()
	{
		return Text::StrToUInt64(this->buff);
	}

	template<class T> UOSInt Text::StringBuilder<T>::Hex2Bytes(OutParam<UInt8> buff)
	{
		return Text::StrHex2Bytes(this->buff, buff);
	}

	template<class T> UnsafeArray<T> Text::StringBuilder<T>::ToString()
	{
		return this->buff;
	}

	template<class T> T *Text::StringBuilder<T>::ToPtr()
	{
		return this->buff.Ptr();
	}

	template<class T> UnsafeArray<T> Text::StringBuilder<T>::GetEndPtr()
	{
		return this->buffEnd;
	}

	template<class T> void Text::StringBuilder<T>::SetEndPtr(UnsafeArray<T> ptr)
	{
		if (ptr < this->buff)
			return;
		if (((UOSInt)(ptr - this->buff) * sizeof(T)) > this->buffSize - sizeof(T))
		{
			return;
		}
		this->buffEnd = ptr;
	}

	template<class T> UOSInt Text::StringBuilder<T>::IndexOf(UnsafeArray<const T> s)
	{
		return Text::StrIndexOf(this->buff, s);
	}

	template<class T> UOSInt Text::StringBuilder<T>::IndexOf(UnsafeArray<const T> s, UOSInt index)
	{
		if (index >= (UOSInt)(this->buffEnd - this->buff))
		{
			return INVALID_INDEX;
		}
		UOSInt retIndex = Text::StrIndexOf(&this->buff[index], s);
		if (retIndex == INVALID_INDEX)
			return INVALID_INDEX;
		return retIndex + index;
	}

	template<class T> UOSInt Text::StringBuilder<T>::IndexOf(T c)
	{
		return Text::StrIndexOfCharC(this->buff, (UOSInt)(this->buffEnd - this->buff), c);
	}

	template<class T> UOSInt Text::StringBuilder<T>::LastIndexOf(T c)
	{
		return Text::StrLastIndexOfCharC(this->buff, (UOSInt)(this->buffEnd - this->buff), c);
	}

	template<class T> Bool Text::StringBuilder<T>::Equals(UnsafeArray<const T> s)
	{
		return Text::StrEquals(this->buff, s);
	}

	template<class T> Bool Text::StringBuilder<T>::EqualsICase(UnsafeArray<const T> s)
	{
		return Text::StrEqualsICase(this->buff, s);
	}

	template<class T> Bool Text::StringBuilder<T>::StartsWith(UnsafeArray<const T> s)
	{
		return Text::StrStartsWith(this->buff, s);
	}

	template<class T> Bool Text::StringBuilder<T>::EndsWith(T c)
	{
		if (this->buff == this->buffEnd)
			return false;
		return this->buffEnd[-1] == c;
	}

	template<class T> UnsafeArray<T> Text::StringBuilder<T>::SubString(UnsafeArray<T> buff, UOSInt start, UOSInt length)
	{
		UOSInt strLen = (UOSInt)(this->buffEnd - this->buff);
		if (start >= strLen)
		{
			*buff = 0;
			return buff;
		}
		UOSInt end = start + length;
		if (end > strLen)
		{
			end = strLen;
		}
		if (end <= start)
		{
			*buff = 0;
			return buff;
		}
		MemCopyNO(buff, &this->buff[start], (end - start) * sizeof(T));
		buff += end - start;
		*buff = 0;
		return buff;
	}

	template <class T> UOSInt Text::StringBuilder<T>::Replace(T fromChar, T toChar)
	{
		return Text::StrReplace(this->buff, fromChar, toChar);
	}

	template<class T> UOSInt Text::StringBuilder<T>::Replace(UnsafeArray<const T> fromStr, UnsafeArray<const T> toStr)
	{
		UOSInt fromCharSize = Text::StrCharCnt(fromStr);
		UOSInt toCharSize = Text::StrCharCnt(toStr);
		UOSInt changeCnt;
		if (fromCharSize >= toCharSize)
		{
			changeCnt = Text::StrReplace(this->buff, fromStr, toStr);
			this->buffEnd += changeCnt * (toCharSize - fromCharSize);
		}
		else
		{
			UOSInt i;
			UOSInt j;
			changeCnt = 0;
			i = 0;
			while (true)
			{
				j = Text::StrIndexOf(&this->buff[i], fromStr);
				if (j == INVALID_INDEX)
					break;
				i += j + fromCharSize;
				changeCnt++;
			}
			this->AllocLeng((UOSInt)(buffEnd - buff) + (toCharSize - fromCharSize) * changeCnt);
			changeCnt = Text::StrReplace(this->buff, fromStr, toStr);
			this->buffEnd += changeCnt * (toCharSize - fromCharSize);
		}
		return changeCnt;
	}

	template<class T> UOSInt Text::StringBuilder<T>::ReplaceICase(UnsafeArray<const T> fromStr, UnsafeArray<const T> toStr)
	{
		UOSInt fromCharSize = Text::StrCharCnt(fromStr);
		UOSInt toCharSize = Text::StrCharCnt(toStr);
		UOSInt changeCnt;
		if (fromCharSize >= toCharSize)
		{
			changeCnt = Text::StrReplaceICase(this->buff, fromStr, toStr);
		}
		else
		{
			UOSInt i;
			UOSInt j;
			changeCnt = 0;
			i = 0;
			while (true)
			{
				j = Text::StrIndexOfICase(&this->buff[i], fromStr);
				if (j == INVALID_INDEX)
					break;
				i += j + fromCharSize;
				changeCnt++;
			}
			this->AllocLeng((buffEnd - buff) + (toCharSize - fromCharSize) * changeCnt);
			changeCnt = Text::StrReplaceICase(this->buff, fromStr, toStr);
		}
		return changeCnt;
	}
}
#endif
