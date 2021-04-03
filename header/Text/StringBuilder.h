#ifndef _SM_TEXT_STRINGBUILDER
#define _SM_TEXT_STRINGBUILDER
#include "MyMemory.h"
#include "Data/DateTime.h"
#include "Text/MyString.h"

namespace Text
{
	template <class T> class StringBuilder
	{
	protected:
		T *buff;
		T *buffEnd;
		UOSInt buffSize;
	public:
		StringBuilder();
		virtual ~StringBuilder();

		StringBuilder<T> *AppendI16(Int16 iVal);
		StringBuilder<T> *AppendU16(UInt16 iVal);
		StringBuilder<T> *AppendI32(Int32 iVal);
		StringBuilder<T> *AppendU32(UInt32 iVal);
		StringBuilder<T> *AppendI64(Int64 iVal);
		StringBuilder<T> *AppendU64(UInt64 iVal);
		StringBuilder<T> *AppendDate(Data::DateTime *dt);
		StringBuilder<T> *AppendOSInt(OSInt iVal);
		StringBuilder<T> *AppendUOSInt(UOSInt iVal);
		StringBuilder<T> *AppendSB(Text::StringBuilder<T> *sb);

		StringBuilder<T> *AppendHex8(UInt8 iVal);
		StringBuilder<T> *AppendHex16(UInt16 iVal);
		StringBuilder<T> *AppendHex24(UInt32 iVal);
		StringBuilder<T> *AppendHex32(UInt32 iVal);
		StringBuilder<T> *AppendHex32V(UInt32 iVal);
		StringBuilder<T> *AppendHex64(UInt64 iVal);
		StringBuilder<T> *AppendHex64V(UInt64 iVal);
		StringBuilder<T> *AppendHexOS(UOSInt iVal);
		StringBuilder<T> *AppendHex(const UInt8 *buff, UOSInt buffSize, T seperator, LineBreakType lineBreak);

		void ClearStr();
		void AllocLeng(UOSInt leng);
		UOSInt GetLength();
		void RemoveChars(UOSInt cnt);
		void Trim();
		void TrimWSCRLF();
		void TrimRight();
		void TrimToLength(UOSInt leng);
		StringBuilder<T> *SetSubstr(OSInt index);
		void ToUpper();
		void ToLower();
		void ToCapital();

		Bool ToUInt16(UInt16 *outVal);
		Bool ToUInt32(UInt32 *outVal);
		Bool ToInt32(Int32 *outVal);
		Int32 ToInt32();
		Bool ToInt64(Int64 *outVal);
		Int64 ToInt64();
		Bool ToOSInt(OSInt *outVal);
		UOSInt Hex2Bytes(UInt8 *buff);

		T *ToString();
		T *GetEndPtr();
		void SetEndPtr(T *ptr);
		OSInt IndexOf(const T *s);
		OSInt IndexOf(const T *s, OSInt index);
		OSInt IndexOf(T c);
		OSInt LastIndexOf(T c);
		Bool Equals(const T *s);
		Bool EqualsICase(const T *s);
		Bool StartsWith(const T *s);
		Bool EndsWith(T c);
		T *SubString(T *buff, OSInt start, OSInt length);
		UOSInt Replace(T fromChar, T toChar);
		UOSInt Replace(const T *fromStr, const T *toStr);
		UOSInt ReplaceICase(const T *fromStr, const T *toStr);
	};

	template <class T> Text::StringBuilder<T>::StringBuilder()
	{
		this->buff = MemAlloc(T, this->buffSize = 1024);
		this->buffEnd = this->buff;
		*buff = 0;
	}
	
	template<class T> Text::StringBuilder<T>::~StringBuilder()
	{
		MemFree(this->buff);
	}

	template <class T> Text::StringBuilder<T> *Text::StringBuilder<T>::AppendI16(Int16 iVal)
	{
		this->AllocLeng(6);
		this->buffEnd = Text::StrInt16(this->buffEnd, iVal);
		return this;
	}

	template <class T> Text::StringBuilder<T> *Text::StringBuilder<T>::AppendU16(UInt16 iVal)
	{
		this->AllocLeng(5);
		this->buffEnd = Text::StrUInt16(this->buffEnd, iVal);
		return this;
	}

	template <class T> Text::StringBuilder<T> *Text::StringBuilder<T>::AppendI32(Int32 iVal)
	{
		this->AllocLeng(11);
		this->buffEnd = Text::StrInt32(this->buffEnd, iVal);
		return this;
	}

	template <class T> Text::StringBuilder<T> *Text::StringBuilder<T>::AppendU32(UInt32 iVal)
	{
		this->AllocLeng(10);
		this->buffEnd = Text::StrUInt32(this->buffEnd, iVal);
		return this;
	}

	template <class T> Text::StringBuilder<T> *Text::StringBuilder<T>::AppendI64(Int64 iVal)
	{
		this->AllocLeng(22);
		this->buffEnd = Text::StrInt64(this->buffEnd, iVal);
		return this;
	}

	template <class T> Text::StringBuilder<T> *Text::StringBuilder<T>::AppendU64(UInt64 iVal)
	{
		this->AllocLeng(20);
		this->buffEnd = Text::StrUInt64(this->buffEnd, iVal);
		return this;
	}

	template <class T> Text::StringBuilder<T> *Text::StringBuilder<T>::AppendDate(Data::DateTime *dt)
	{
		Char sbuff[30];
		dt->ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
		this->AllocLeng(19);
		this->buffEnd = Text::StrConcatASCII(this->buffEnd, sbuff);
		return this;
	}

	template <class T> Text::StringBuilder<T> *Text::StringBuilder<T>::AppendOSInt(OSInt iVal)
	{
#if _OSINT_SIZE == 64
		this->AllocLeng(22);
		this->buffEnd = Text::StrInt64(this->buffEnd, iVal);
#else
		this->AllocLeng(11);
		this->buffEnd = Text::StrInt32(this->buffEnd, (Int32)iVal);
#endif
		return this;
	}

	template <class T> Text::StringBuilder<T> *Text::StringBuilder<T>::AppendUOSInt(UOSInt iVal)
	{
#if _OSINT_SIZE == 64
		this->AllocLeng(22);
		this->buffEnd = Text::StrUInt64(this->buffEnd, iVal);
#else
		this->AllocLeng(11);
		this->buffEnd = Text::StrUInt32(this->buffEnd, (UInt32)iVal);
#endif
		return this;
	}

	template <class T> Text::StringBuilder<T> *Text::StringBuilder<T>::AppendSB(Text::StringBuilder<T> *sb)
	{
		OSInt slen = sb->buffEnd - sb->buff;
		this->AllocLeng(slen);
		MemCopyNO(this->buffEnd, sb->buff, (slen + 1) * sizeof(T));
		this->buffEnd = &this->buffEnd[slen];
		return this;
	}

	template <class T> Text::StringBuilder<T> *Text::StringBuilder<T>::AppendHex8(UInt8 iVal)
	{
		this->AllocLeng(2);
		this->buffEnd = Text::StrHexByte(this->buffEnd, iVal);
		return this;
	}

	template <class T> Text::StringBuilder<T> *Text::StringBuilder<T>::AppendHex16(UInt16 iVal)
	{
		this->AllocLeng(4);
		this->buffEnd = Text::StrHexVal16(this->buffEnd, iVal);
		return this;
	}

	template <class T> Text::StringBuilder<T> *Text::StringBuilder<T>::AppendHex24(UInt32 iVal)
	{
		this->AllocLeng(6);
		this->buffEnd = Text::StrHexVal24(this->buffEnd, iVal);
		return this;
	}

	template <class T> Text::StringBuilder<T> *Text::StringBuilder<T>::AppendHex32(UInt32 iVal)
	{
		this->AllocLeng(8);
		this->buffEnd = Text::StrHexVal32(this->buffEnd, iVal);
		return this;
	}

	template <class T> Text::StringBuilder<T> *Text::StringBuilder<T>::AppendHex32V(UInt32 iVal)
	{
		this->AllocLeng(8);
		this->buffEnd = Text::StrHexVal32V(this->buffEnd, iVal);
		return this;
	}

	template <class T> Text::StringBuilder<T> *Text::StringBuilder<T>::AppendHex64(UInt64 iVal)
	{
		this->AllocLeng(16);
		this->buffEnd = Text::StrHexVal64(this->buffEnd, iVal);
		return this;
	}

	template <class T> Text::StringBuilder<T> *Text::StringBuilder<T>::AppendHex64V(UInt64 iVal)
	{
		this->AllocLeng(16);
		this->buffEnd = Text::StrHexVal64V(this->buffEnd, iVal);
		return this;
	}

	template <class T> Text::StringBuilder<T> *Text::StringBuilder<T>::AppendHexOS(UOSInt iVal)
	{
	#if _OSINT_SIZE == 64
		return AppendHex64(iVal);
	#elif _OSINT_SIZE == 32
		return AppendHex32((UInt32)iVal);
	#elif _OSINT_SIZE == 16
		return AppendHex16((Int32)iVal);
	#endif
	}

	template <class T> Text::StringBuilder<T> *Text::StringBuilder<T>::AppendHex(const UInt8 *buff, UOSInt buffSize, T seperator, LineBreakType lineBreak)
	{
		UOSInt lbCnt = (buffSize >> 4);
		OSInt i;
		if ((buffSize & 15) == 0)
			lbCnt -= 1;
		if (lineBreak == LBT_CRLF)
			lbCnt = lbCnt << 2;
		else if (lineBreak == LBT_CR || lineBreak == LBT_LF)
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
				this->buffEnd = Text::StrHexByte(this->buffEnd, *buff++);
				i++;
				if ((i & 15) == 0 && buffSize > 0)
				{
					if (lineBreak == LBT_CRLF)
					{
						*buffEnd++ = '\r';
						*buffEnd++ = '\n';
					}
					else if (lineBreak == LBT_CR)
					{
						*buffEnd++ = '\r';
					}
					else if (lineBreak == LBT_LF)
					{
						*buffEnd++ = '\n';
					}
				}
			}
		}
		else
		{
			this->AllocLeng(buffSize * 3 + lbCnt - 1);
			while (buffSize-- > 0)
			{
				this->buffEnd = Text::StrHexByte(this->buffEnd, *buff++);
				*buffEnd++ = seperator;
				i++;
				if ((i & 15) == 0 && buffSize > 0)
				{
					if (lineBreak == LBT_CRLF)
					{
						*buffEnd++ = '\r';
						*buffEnd++ = '\n';
					}
					else if (lineBreak == LBT_CR)
					{
						*buffEnd++ = '\r';
					}
					else if (lineBreak == LBT_LF)
					{
						*buffEnd++ = '\n';
					}
				}
			}
			*--buffEnd = 0;
		}
		return this;
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
			T *newStr = MemAlloc(T, this->buffSize);
			MemCopyNO(newStr, this->buff, currSize);
			this->buffEnd = &newStr[(currSize / sizeof(T)) - 1];
			MemFree(this->buff);
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
		*buffEnd = 0;
	}

	template<class T> void Text::StringBuilder<T>::Trim()
	{
		this->buffEnd = Text::StrTrim(this->buff);
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
			*this->buffEnd = 0;
		}
	}

	template<class T> Text::StringBuilder<T> *Text::StringBuilder<T>::SetSubstr(OSInt index)
	{
		if (index >= (this->buffEnd - this->buff))
		{
			buffEnd = buff;
			*buff = 0;
		}
		else if (index > 0)
		{
			MemCopyO(this->buff, &this->buff[index], (this->buffEnd - this->buff - index + 1) * sizeof(T));
			this->buffEnd -= index;
		}
		return this;
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

	template<class T> Bool Text::StringBuilder<T>::ToUInt16(UInt16 *outVal)
	{
		return Text::StrToUInt16(this->buff, outVal);
	}

	template<class T> Bool Text::StringBuilder<T>::ToUInt32(UInt32 *outVal)
	{
		return Text::StrToUInt32(this->buff, outVal);
	}

	template<class T> Bool Text::StringBuilder<T>::ToInt32(Int32 *outVal)
	{
		return Text::StrToInt32(this->buff, outVal);
	}

	template<class T> Bool Text::StringBuilder<T>::ToInt64(Int64 *outVal)
	{
		return Text::StrToInt64(this->buff, outVal);
	}

	template<class T> Bool Text::StringBuilder<T>::ToOSInt(OSInt *outVal)
	{
		return Text::StrToOSInt(this->buff, outVal);
	}

	template<class T> Int32 Text::StringBuilder<T>::ToInt32()
	{
		return Text::StrToInt32(this->buff);
	}

	template<class T> Int64 Text::StringBuilder<T>::ToInt64()
	{
		return Text::StrToInt64(this->buff);
	}

	template<class T> UOSInt Text::StringBuilder<T>::Hex2Bytes(UInt8 *buff)
	{
		return Text::StrHex2Bytes(this->buff, buff);
	}

	template<class T> T *Text::StringBuilder<T>::ToString()
	{
		return this->buff;
	}

	template<class T> T *Text::StringBuilder<T>::GetEndPtr()
	{
		return this->buffEnd;
	}

	template<class T> void Text::StringBuilder<T>::SetEndPtr(T *ptr)
	{
		if (ptr < this->buff)
			return;
		if (((ptr - this->buff) * sizeof(T)) > this->buffSize - sizeof(T))
		{
			return;
		}
		this->buffEnd = ptr;
	}

	template<class T> OSInt Text::StringBuilder<T>::IndexOf(const T *s)
	{
		return Text::StrIndexOf(this->buff, s);
	}

	template<class T> OSInt Text::StringBuilder<T>::IndexOf(const T *s, OSInt index)
	{
		if (index < 0)
			index = 0;
		if (index >= (this->buffEnd - this->buff))
		{
			return -1;
		}
		OSInt retIndex = Text::StrIndexOf(&this->buff[index], s);
		if (retIndex < 0)
			return -1;
		return retIndex + index;
	}

	template<class T> OSInt Text::StringBuilder<T>::IndexOf(T c)
	{
		return Text::StrIndexOf(this->buff, c);
	}

	template<class T> OSInt Text::StringBuilder<T>::LastIndexOf(T c)
	{
		return Text::StrLastIndexOf(this->buff, c);
	}

	template<class T> Bool Text::StringBuilder<T>::Equals(const T *s)
	{
		return Text::StrEquals(this->buff, s);
	}

	template<class T> Bool Text::StringBuilder<T>::EqualsICase(const T *s)
	{
		return Text::StrEqualsICase(this->buff, s);
	}

	template<class T> Bool Text::StringBuilder<T>::StartsWith(const T *s)
	{
		return Text::StrStartsWith(this->buff, s);
	}

	template<class T> Bool Text::StringBuilder<T>::EndsWith(T c)
	{
		if (this->buff == this->buffEnd)
			return false;
		return this->buffEnd[-1] == c;
	}

	template<class T> T *Text::StringBuilder<T>::SubString(T *buff, OSInt start, OSInt length)
	{
		OSInt strLen = (this->buffEnd - this->buff);
		if (start >= strLen)
		{
			*buff = 0;
			return buff;
		}
		else if (start < 0)
		{
			length += start;
			start = 0;
		}
		OSInt end = start + length;
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

	template<class T> UOSInt Text::StringBuilder<T>::Replace(const T *fromStr, const T *toStr)
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
			OSInt j;
			changeCnt = 0;
			i = 0;
			while (true)
			{
				j = Text::StrIndexOf(&this->buff[i], fromStr);
				if (j < 0)
					break;
				i += j + fromCharSize;
				changeCnt++;
			}
			this->AllocLeng((buffEnd - buff) + (toCharSize - fromCharSize) * changeCnt);
			changeCnt = Text::StrReplace(this->buff, fromStr, toStr);
			this->buffEnd += changeCnt * (toCharSize - fromCharSize);
		}
		return changeCnt;
	}

	template<class T> UOSInt Text::StringBuilder<T>::ReplaceICase(const T *fromStr, const T *toStr)
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
			OSInt j;
			changeCnt = 0;
			i = 0;
			while (true)
			{
				j = Text::StrIndexOfICase(&this->buff[i], fromStr);
				if (j < 0)
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
