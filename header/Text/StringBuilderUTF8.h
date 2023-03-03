#ifndef _SM_TEXT_STRINGBUILDERUTF8
#define _SM_TEXT_STRINGBUILDERUTF8
#include "MyMemory.h"
#include "Data/DateTime.h"
#include "Data/Timestamp.h"
#include "Text/LineBreakType.h"
#include "Text/PString.h"

namespace Text
{
	class StringBuilderUTF8 : public Text::PString
	{
	private:
		UOSInt buffSize;
	public:
		StringBuilderUTF8()
		{
			this->buffSize = 1024;
			this->v = MemAlloc(UTF8Char, this->buffSize + 1);
			this->leng = 0;
			this->v[0] = 0;
		}

		~StringBuilderUTF8()
		{
			MemFree(this->v);
		}

		void AllocLeng(UOSInt leng)
		{
			if (leng + this->leng > this->buffSize)
			{
				this->buffSize <<= 1;
				while (leng + this->leng > this->buffSize)
				{
					this->buffSize <<= 1;
				}
				UTF8Char *newStr = MemAlloc(UTF8Char, this->buffSize + 1);
				MemCopyNO(newStr, this->v, this->leng + 1);
				MemFree(this->v);
				this->v = newStr;
			}
		}

		StringBuilderUTF8 *Append(Text::StringBase<UTF8Char> *s);
		StringBuilderUTF8 *Append(Text::StringBase<const UTF8Char> *s);
		StringBuilderUTF8 *Append(const Text::StringBase<UTF8Char> &s);
		StringBuilderUTF8 *Append(const Text::StringBase<const UTF8Char> &s);
		StringBuilderUTF8 *AppendW(const WChar *s);
		StringBuilderUTF8 *AppendW(const WChar *s, UOSInt len);
		StringBuilderUTF8 *AppendUTF16(const UTF16Char *s);
		StringBuilderUTF8 *AppendUTF16(const UTF16Char *s, UOSInt utf16Cnt);
		StringBuilderUTF8 *AppendUTF16BE(const UInt8 *s, UOSInt utf16Cnt);
		StringBuilderUTF8 *AppendUTF32(const UTF32Char *s);
		StringBuilderUTF8 *AppendUTF32(const UTF32Char *s, UOSInt utf32Cnt);
		StringBuilderUTF8 *AppendSlow(const UTF8Char *s);

		StringBuilderUTF8 *AppendS(const UTF8Char *s, UOSInt maxLen);
		StringBuilderUTF8 *AppendUTF8Char(UTF8Char c)
		{
			this->AllocLeng(1);
			this->v[this->leng] = c;
			this->v[this->leng + 1] = 0;
			this->leng++;
			return this;
		}

		StringBuilderUTF8 *AppendChar(UTF32Char c, UOSInt repCnt);

		StringBuilderUTF8 *AppendNE(const UTF8Char *s, UOSInt charCnt)
		{
			this->AllocLeng(charCnt);
			MemCopyNOShort(&this->v[this->leng], s, charCnt);
			this->leng += charCnt;
			return this;
		}

		StringBuilderUTF8 *AppendNE2(const UTF8Char *str1, UOSInt len1, const UTF8Char *str2, UOSInt len2)
		{
			UOSInt tlen = len1 + len2;
			this->AllocLeng(tlen);
			UTF8Char *dptr = &this->v[this->leng];
			MemCopyNOShort(dptr, str1, len1);
			dptr += len1;
			MemCopyNOShort(dptr, str2, len2);
			dptr += len2;
			this->leng += tlen;
			return this;
		}

		StringBuilderUTF8 *AppendNE(Text::StringBase<UTF8Char> *s)
		{
			UOSInt charCnt = s->leng;
			this->AllocLeng(charCnt);
			MemCopyNOShort(&this->v[this->leng], s->v, charCnt);
			this->leng += charCnt;
			return this;
		}

		StringBuilderUTF8 *AppendC(const UTF8Char *s, UOSInt charCnt)
		{
			this->AllocLeng(charCnt);
			MemCopyNOShort(&this->v[this->leng], s, charCnt);
			this->leng += charCnt;
			this->v[this->leng] = 0;
			return this;
		}

		StringBuilderUTF8 *AppendC2(const UTF8Char *str1, UOSInt len1, const UTF8Char *str2, UOSInt len2)
		{
			UOSInt tlen = len1 + len2;
			this->AllocLeng(tlen);
			UTF8Char *dptr = &this->v[this->leng];
			MemCopyNOShort(dptr, str1, len1);
			dptr += len1;
			MemCopyNOShort(dptr, str2, len2);
			dptr += len2;
			*dptr = 0;
			this->leng += tlen;
			return this;
		}

		StringBuilderUTF8 *AppendP(const UTF8Char *str, const UTF8Char *strEnd)
		{
			return AppendC(str, (UOSInt)(strEnd - str));
		}

		StringBuilderUTF8 *AppendI16(Int16 iVal)
		{
			this->AllocLeng(6);
			this->leng = (UOSInt)(Text::StrInt16(&this->v[this->leng], iVal) - this->v);
			return this;
		}

		StringBuilderUTF8 *AppendU16(UInt16 iVal)
		{
			this->AllocLeng(5);
			this->leng = (UOSInt)(Text::StrUInt16(&this->v[this->leng], iVal) - this->v);
			return this;
		}

		StringBuilderUTF8 *AppendI32(Int32 iVal)
		{
			this->AllocLeng(11);
			this->leng = (UOSInt)(Text::StrInt32(&this->v[this->leng], iVal) - this->v);
			return this;
		}

		StringBuilderUTF8 *AppendU32(UInt32 iVal)
		{
			this->AllocLeng(10);
			this->leng = (UOSInt)(Text::StrUInt32(&this->v[this->leng], iVal) - this->v);
			return this;
		}

		StringBuilderUTF8 *AppendI64(Int64 iVal)
		{
			this->AllocLeng(22);
			this->leng = (UOSInt)(Text::StrInt64(&this->v[this->leng], iVal) - this->v);
			return this;
		}

		StringBuilderUTF8 *AppendU64(UInt64 iVal)
		{
			this->AllocLeng(20);
			this->leng = (UOSInt)(Text::StrUInt64(&this->v[this->leng], iVal) - this->v);
			return this;
		}

		StringBuilderUTF8 *AppendDouble(Double dVal)
		{
			this->AllocLeng(32);
			this->leng = (UOSInt)(Text::StrDouble(&this->v[this->leng], dVal) - this->v);
			return this;
		}

		StringBuilderUTF8 *AppendDouble(Double dVal, const DoubleStyle *style)
		{
			this->AllocLeng(32);
			this->leng = (UOSInt)(Text::StrDouble(&this->v[this->leng], dVal, style) - this->v);
			return this;
		}

		StringBuilderUTF8 *AppendDate(Data::DateTime *dt)
		{
			this->AllocLeng(19);
			this->leng = (UOSInt)(dt->ToString(&this->v[this->leng], "yyyy-MM-dd HH:mm:ss") - this->v);
			return this;
		}

		StringBuilderUTF8 *AppendTS(const Data::Timestamp &ts)
		{
			if (ts.inst.nanosec == 0)
			{
				this->AllocLeng(19);
				this->leng = (UOSInt)(ts.ToString(&this->v[this->leng], "yyyy-MM-dd HH:mm:ss") - this->v);
			}
			else if (ts.inst.nanosec % 1000000 == 0)
			{
				this->AllocLeng(23);
				this->leng = (UOSInt)(ts.ToString(&this->v[this->leng], "yyyy-MM-dd HH:mm:ss.fff") - this->v);
			}
			else if (ts.inst.nanosec % 1000 == 0)
			{
				this->AllocLeng(26);
				this->leng = (UOSInt)(ts.ToString(&this->v[this->leng], "yyyy-MM-dd HH:mm:ss.ffffff") - this->v);
			}
			else
			{
				this->AllocLeng(29);
				this->leng = (UOSInt)(ts.ToString(&this->v[this->leng], "yyyy-MM-dd HH:mm:ss.fffffffff") - this->v);
			}
			return this;
		}

		StringBuilderUTF8 *AppendOSInt(OSInt iVal)
		{
	#if _OSINT_SIZE == 64
			this->AllocLeng(22);
			this->leng = (UOSInt)(Text::StrInt64(&this->v[this->leng], iVal) - this->v);
	#else
			this->AllocLeng(11);
			this->leng = (UOSInt)(Text::StrInt32(&this->v[this->leng], (Int32)iVal) - this->v);
	#endif
			return this;
		}

		StringBuilderUTF8 *AppendUOSInt(UOSInt iVal)
		{
	#if _OSINT_SIZE == 64
			this->AllocLeng(22);
			this->leng = (UOSInt)(Text::StrUInt64(&this->v[this->leng], iVal) - this->v);
	#else
			this->AllocLeng(11);
			this->leng = (UOSInt)(Text::StrUInt32(&this->v[this->leng], (UInt32)iVal) - this->v);
	#endif
			return this;
		}

		StringBuilderUTF8 *AppendSB(Text::StringBuilderUTF8 *sb)
		{
			return Append(sb);
		}

		StringBuilderUTF8 *AppendHex8(UInt8 iVal)
		{
			this->AllocLeng(2);
			this->leng = (UOSInt)(Text::StrHexByte(&this->v[this->leng], iVal) - this->v);
			return this;
		}

		StringBuilderUTF8 *AppendHex16(UInt16 iVal)
		{
			this->AllocLeng(4);
			this->leng = (UOSInt)(Text::StrHexVal16(&this->v[this->leng], iVal) - this->v);
			return this;
		}

		StringBuilderUTF8 *AppendHex24(UInt32 iVal)
		{
			this->AllocLeng(6);
			this->leng = (UOSInt)(Text::StrHexVal24(&this->v[this->leng], iVal) - this->v);
			return this;
		}

		StringBuilderUTF8 *AppendHex32(UInt32 iVal)
		{
			this->AllocLeng(8);
			this->leng = (UOSInt)(Text::StrHexVal32(&this->v[this->leng], iVal) - this->v);
			return this;
		}

		StringBuilderUTF8 *AppendHex32V(UInt32 iVal)
		{
			this->AllocLeng(8);
			this->leng = (UOSInt)(Text::StrHexVal32V(&this->v[this->leng], iVal) - this->v);
			return this;
		}

		StringBuilderUTF8 *AppendHex64(UInt64 iVal)
		{
			this->AllocLeng(16);
			this->leng = (UOSInt)(Text::StrHexVal64(&this->v[this->leng], iVal) - this->v);
			return this;
		}

		StringBuilderUTF8 *AppendHex64V(UInt64 iVal)
		{
			this->AllocLeng(16);
			this->leng = (UOSInt)(Text::StrHexVal64V(&this->v[this->leng], iVal) - this->v);
			return this;
		}

		StringBuilderUTF8 *AppendHexOS(UOSInt iVal)
		{
		#if _OSINT_SIZE == 64
			return AppendHex64(iVal);
		#elif _OSINT_SIZE == 32
			return AppendHex32((UInt32)iVal);
		#elif _OSINT_SIZE == 16
			return AppendHex16((Int32)iVal);
		#endif
		}

		StringBuilderUTF8 *AppendHexBuff(const UInt8 *buff, UOSInt buffSize, UTF8Char seperator, Text::LineBreakType lineBreak)
		{
			if (buffSize == 0)
				return this;
			UOSInt lbCnt;
			UOSInt lineCnt;
			OSInt i;
			if (lineBreak == LineBreakType::None)
			{
				lbCnt = 0;
				lineCnt = 0;
			}
			else
			{
				lineCnt = (buffSize >> 4);
				if ((buffSize & 15) == 0)
					lineCnt -= 1;
				if (lineBreak == LineBreakType::CRLF)
					lbCnt = lineCnt << 1;
				else
					lbCnt = lineCnt;
			}
			i = 0;
			if (seperator == 0)
			{
				this->AllocLeng((buffSize << 1) + lbCnt);
				UTF8Char *buffEnd = &this->v[this->leng];
				this->leng += (buffSize << 1) + lbCnt;
				while (buffSize-- > 0)
				{
					buffEnd[0] = (UTF8Char)MyString_STRHEXARR[*buff >> 4];
					buffEnd[1] = (UTF8Char)MyString_STRHEXARR[*buff & 15];
					buffEnd += 2;
					buff++;
					i++;
					if ((i & 15) == 0 && buffSize > 0)
					{
						if (lineBreak == LineBreakType::CRLF)
						{
							buffEnd[0] = 13;
							buffEnd[1] = 10;
							buffEnd += 2;
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
				buffEnd[0] = 0;
			}
			else
			{
				this->AllocLeng(buffSize * 3 + lbCnt - 1 - lineCnt);
				UTF8Char *buffEnd = &this->v[this->leng];
				this->leng += buffSize * 3 + lbCnt - 1 - lineCnt;
				while (buffSize-- > 0)
				{
					i++;
					buffEnd[0] = (UTF8Char)MyString_STRHEXARR[*buff >> 4];
					buffEnd[1] = (UTF8Char)MyString_STRHEXARR[*buff & 15];
					buff++;
					if (buffSize > 0)
					{
						if ((i & 15) == 0)
						{
							switch (lineBreak)
							{
							case LineBreakType::CRLF:
								buffEnd[2] = 13;
								buffEnd[3] = 10;
								buffEnd += 4;
								break;
							case LineBreakType::CR:
								buffEnd[2] = '\r';
								buffEnd += 3;
								break;
							case LineBreakType::LF:
								buffEnd[2] = '\n';
								buffEnd += 3;
								break;
							case LineBreakType::None:
							default:
								buffEnd[2] = seperator;
								buffEnd += 3;
								break;
							}
						}
						else
						{
							buffEnd[2] = seperator;
							buffEnd += 3;
						}
					}
					else
					{
						buffEnd += 2;
						*buffEnd = 0;
						break;
					}
				}
			}
			return this;
		}

		StringBuilderUTF8 *AppendHex8LC(UInt8 iVal)
		{
			this->AllocLeng(2);
			this->leng = (UOSInt)(Text::StrHexByteLC(&this->v[this->leng], iVal) - this->v);
			return this;
		}

		StringBuilderUTF8 *AppendHex16LC(UInt16 iVal)
		{
			this->AllocLeng(4);
			this->leng = (UOSInt)(Text::StrHexVal16LC(&this->v[this->leng], iVal) - this->v);
			return this;
		}

		StringBuilderUTF8 *AppendHex24LC(UInt32 iVal)
		{
			this->AllocLeng(6);
			this->leng = (UOSInt)(Text::StrHexVal24LC(&this->v[this->leng], iVal) - this->v);
			return this;
		}

		StringBuilderUTF8 *AppendHex32LC(UInt32 iVal)
		{
			this->AllocLeng(8);
			this->leng = (UOSInt)(Text::StrHexVal32LC(&this->v[this->leng], iVal) - this->v);
			return this;
		}

		StringBuilderUTF8 *AppendHex32VLC(UInt32 iVal)
		{
			this->AllocLeng(8);
			this->leng = (UOSInt)(Text::StrHexVal32VLC(&this->v[this->leng], iVal) - this->v);
			return this;
		}

		StringBuilderUTF8 *AppendHex64LC(UInt64 iVal)
		{
			this->AllocLeng(16);
			this->leng = (UOSInt)(Text::StrHexVal64LC(&this->v[this->leng], iVal) - this->v);
			return this;
		}

		StringBuilderUTF8 *AppendHex64VLC(UInt64 iVal)
		{
			this->AllocLeng(16);
			this->leng = (UOSInt)(Text::StrHexVal64VLC(&this->v[this->leng], iVal) - this->v);
			return this;
		}

		StringBuilderUTF8 *AppendHexOSLC(UOSInt iVal)
		{
		#if _OSINT_SIZE == 64
			return AppendHex64LC(iVal);
		#elif _OSINT_SIZE == 32
			return AppendHex32LC((UInt32)iVal);
		#elif _OSINT_SIZE == 16
			return AppendHex16LC((Int32)iVal);
		#endif
		}

		StringBuilderUTF8 *AppendHexBuffLC(const UInt8 *buff, UOSInt buffSize, UTF8Char seperator, Text::LineBreakType lineBreak)
		{
			if (buffSize == 0)
				return this;
			UOSInt lbCnt;
			UOSInt lineCnt;
			OSInt i;
			if (lineBreak == LineBreakType::None)
			{
				lbCnt = 0;
				lineCnt = 0;
			}
			else
			{
				lineCnt = (buffSize >> 4);
				if ((buffSize & 15) == 0)
					lineCnt -= 1;
				if (lineBreak == LineBreakType::CRLF)
					lbCnt = lineCnt << 1;
				else
					lbCnt = lineCnt;
			}
			i = 0;
			if (seperator == 0)
			{
				this->AllocLeng((buffSize << 1) + lbCnt);
				UTF8Char *buffEnd = &this->v[this->leng];
				this->leng += (buffSize << 1) + lbCnt;
				while (buffSize-- > 0)
				{
					buffEnd[0] = (UTF8Char)MyString_STRhexarr[*buff >> 4];
					buffEnd[1] = (UTF8Char)MyString_STRhexarr[*buff & 15];
					buffEnd += 2;
					buff++;
					i++;
					if ((i & 15) == 0 && buffSize > 0)
					{
						if (lineBreak == LineBreakType::CRLF)
						{
							buffEnd[0] = 13;
							buffEnd[1] = 10;
							buffEnd += 2;
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
				buffEnd[0] = 0;
			}
			else
			{
				this->AllocLeng(buffSize * 3 + lbCnt - 1 - lineCnt);
				UTF8Char *buffEnd = &this->v[this->leng];
				this->leng += buffSize * 3 + lbCnt - 1 - lineCnt;
				while (buffSize-- > 0)
				{
					i++;
					buffEnd[0] = (UTF8Char)MyString_STRhexarr[*buff >> 4];
					buffEnd[1] = (UTF8Char)MyString_STRhexarr[*buff & 15];
					buff++;
					if (buffSize > 0)
					{
						if ((i & 15) == 0)
						{
							switch (lineBreak)
							{
							case LineBreakType::CRLF:
								buffEnd[2] = 13;
								buffEnd[3] = 10;
								buffEnd += 4;
								break;
							case LineBreakType::CR:
								buffEnd[2] = '\r';
								buffEnd += 3;
								break;
							case LineBreakType::LF:
								buffEnd[2] = '\n';
								buffEnd += 3;
								break;
							case LineBreakType::None:
							default:
								buffEnd[2] = seperator;
								buffEnd += 3;
								break;
							}
						}
						else
						{
							buffEnd[2] = seperator;
							buffEnd += 3;
						}
					}
					else
					{
						buffEnd += 2;
						*buffEnd = 0;
						break;
					}
				}
			}
			return this;
		}

		StringBuilderUTF8 *AppendLB(Text::LineBreakType lbt)
		{
			switch (lbt)
			{
 			case Text::LineBreakType::CRLF:
				return AppendC(UTF8STRC("\r\n"));
			case Text::LineBreakType::CR:
				return AppendUTF8Char('\r');
			case Text::LineBreakType::LF:
				return AppendUTF8Char('\n');
			case Text::LineBreakType::None:
			default:
				return this;
			}
		}
		StringBuilderUTF8 *AppendCSV(const UTF8Char **sarr, UOSInt nStr);
		StringBuilderUTF8 *AppendToUpper(const UTF8Char *s, UOSInt len);
		StringBuilderUTF8 *AppendToLower(const UTF8Char *s, UOSInt len);
		StringBuilderUTF8 *RemoveANSIEscapes();

		StringBuilderUTF8 *SetSubstr(UOSInt index)
		{
			if (index >= this->leng)
			{
				this->leng = 0;
				this->v[0] = 0;
			}
			else if (index > 0)
			{
				MemCopyO(this->v, &this->v[index], this->leng - index + 1);
				this->leng -= index;
			}
			return this;
		}

		UOSInt ReplaceStr(const UTF8Char *fromStr, UOSInt fromLen, const UTF8Char *toStr, UOSInt toLen)
		{
			UOSInt changeCnt;
			if (fromLen >= toLen)
			{
				changeCnt = Text::StrReplace(this->v, fromStr, toStr);
				this->leng += (UOSInt)((OSInt)changeCnt * (OSInt)(toLen - fromLen));
			}
			else
			{
				UOSInt i;
				UOSInt j;
				changeCnt = 0;
				i = 0;
				while (true)
				{
					j = Text::StrIndexOfC(&this->v[i], this->leng - i, fromStr, fromLen);
					if (j == INVALID_INDEX)
						break;
					i += j + fromLen;
					changeCnt++;
				}
				if (changeCnt > 0)
				{
					this->AllocLeng(this->leng + (toLen - fromLen) * changeCnt);
					changeCnt = Text::StrReplace(this->v, fromStr, toStr);
					this->leng += changeCnt * (toLen - fromLen);
				}
			}
			return changeCnt;
		}

		void ClearStr()
		{
			this->leng = 0;
			this->v[0] = 0;
		}

		const UTF8Char *ToString() const
		{
			return this->v;
		}

		UOSInt GetLength() const
		{
			return this->leng;
		}

		UOSInt GetCharCnt() const
		{
			return this->leng;
		}

		void SetEndPtr(UTF8Char *endPtr)
		{
			this->leng = (UOSInt)(endPtr - this->v);
		}
 	};

	FORCEINLINE void SBAppendF32(Text::StringBuilderUTF8 *sb, Single v)
	{
		sb->AppendDouble(v);
	}	

	FORCEINLINE void SBAppendF64(Text::StringBuilderUTF8 *sb, Double v)
	{
		sb->AppendDouble(v);
	}	
}
#endif
