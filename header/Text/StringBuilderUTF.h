#ifndef _SM_TEXT_STRINGBUILDERUTF
#define _SM_TEXT_STRINGBUILDERUTF
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilder.h"

namespace Text
{
	class StringBuilderUTF
	{
	public:
		virtual void ClearStr() = 0;
		virtual void AllocLeng(UOSInt leng) = 0;
		virtual UOSInt GetCharCnt() = 0;
		virtual Bool EndsWith(Char c) = 0;
		virtual void RemoveChars(UOSInt cnt) = 0;
		virtual void RemoveChars(UOSInt index, UOSInt cnt) = 0;

		virtual StringBuilderUTF *AppendI16(Int16 iVal) = 0;
		virtual StringBuilderUTF *AppendU16(UInt16 iVal) = 0;
		virtual StringBuilderUTF *AppendI32(Int32 iVal) = 0;
		virtual StringBuilderUTF *AppendU32(UInt32 iVal) = 0;
		virtual StringBuilderUTF *AppendI64(Int64 iVal) = 0;
		virtual StringBuilderUTF *AppendU64(UInt64 iVal) = 0;
		virtual StringBuilderUTF *AppendDate(Data::DateTime *dt) = 0;
		virtual StringBuilderUTF *AppendOSInt(OSInt iVal) = 0;
		virtual StringBuilderUTF *AppendUOSInt(UOSInt iVal) = 0;
		virtual StringBuilderUTF *AppendLB(Text::LineBreakType lbt) = 0;

		virtual StringBuilderUTF *AppendHex8(UInt8 iVal) = 0;
		virtual StringBuilderUTF *AppendHex16(UInt16 iVal) = 0;
		virtual StringBuilderUTF *AppendHex24(UInt32 iVal) = 0;
		virtual StringBuilderUTF *AppendHex32(UInt32 iVal) = 0;
		virtual StringBuilderUTF *AppendHex32V(UInt32 iVal) = 0;
		virtual StringBuilderUTF *AppendHex64(UInt64 iVal) = 0;
		virtual StringBuilderUTF *AppendHex64V(UInt64 iVal) = 0;
		virtual StringBuilderUTF *AppendHexOS(UOSInt iVal) = 0;
		virtual StringBuilderUTF *AppendHexBuff(const UInt8 *buff, UOSInt buffSize, UTF32Char seperator, Text::LineBreakType lineBreak) = 0;

		virtual StringBuilderUTF *Append(const UTF8Char *s) = 0;
		virtual StringBuilderUTF *AppendC(const UTF8Char *s, UOSInt charCnt) = 0;
		virtual StringBuilderUTF *AppendS(const UTF8Char *s, UOSInt maxLen) = 0;
		virtual StringBuilderUTF *AppendChar(UTF32Char c, UOSInt repCnt) = 0;
	};

	template <class T> class StringBuilderUTFText : public Text::StringBuilder<T>, public Text::StringBuilderUTF
	{
	public:
		virtual void ClearStr() { ::Text::StringBuilder<T>::ClearStr(); }
		virtual void AllocLeng(UOSInt leng) { ::Text::StringBuilder<T>::AllocLeng(leng); }
		virtual UOSInt GetCharCnt() { return this->GetLength(); }
		virtual Bool EndsWith(Char c) { if (this->buff == this->buffEnd) return false; return this->buffEnd[-1] == (T)c; }
		virtual Bool EndsWith(const T *s) { UOSInt l = Text::StrCharCnt(s); if (this->GetLength() < l) return false; return Text::StrEquals((const T*)&this->buffEnd[-(OSInt)l], s); }
		virtual void RemoveChars(UOSInt cnt) {::Text::StringBuilder<T>::RemoveChars(cnt); }
		virtual void RemoveChars(UOSInt index, UOSInt cnt) {::Text::StringBuilder<T>::RemoveChars(index, cnt); }
		virtual StringBuilderUTF *AppendI16(Int16 iVal) { return (StringBuilderUTF*)::Text::StringBuilder<T>::AppendI16(iVal); }
		virtual StringBuilderUTF *AppendU16(UInt16 iVal) { return (StringBuilderUTF*)::Text::StringBuilder<T>::AppendU16(iVal); }
		virtual StringBuilderUTF *AppendI32(Int32 iVal) { return (StringBuilderUTF*)::Text::StringBuilder<T>::AppendI32(iVal); }
		virtual StringBuilderUTF *AppendU32(UInt32 iVal) { return (StringBuilderUTF*)::Text::StringBuilder<T>::AppendU32(iVal); }
		virtual StringBuilderUTF *AppendI64(Int64 iVal) { return (StringBuilderUTF*)::Text::StringBuilder<T>::AppendI64(iVal); }
		virtual StringBuilderUTF *AppendU64(UInt64 iVal) { return (StringBuilderUTF*)::Text::StringBuilder<T>::AppendU64(iVal); }
		virtual StringBuilderUTF *AppendDate(Data::DateTime *dt) { return (StringBuilderUTF*)::Text::StringBuilder<T>::AppendDate(dt); }
		virtual StringBuilderUTF *AppendOSInt(OSInt iVal) { return (StringBuilderUTF*)::Text::StringBuilder<T>::AppendOSInt(iVal); }
		virtual StringBuilderUTF *AppendUOSInt(UOSInt iVal) { return (StringBuilderUTF*)::Text::StringBuilder<T>::AppendUOSInt(iVal); }
		virtual StringBuilderUTF *AppendLB(Text::LineBreakType lbt) {if (lbt == Text::LBT_CRLF) return Append((const UTF8Char*)"\r\n"); else if (lbt == Text::LBT_CR) return AppendChar('\r', 1); else if (lbt == Text::LBT_LF) return AppendChar('\n', 1); else return this; }

		virtual StringBuilderUTF *AppendHex8(UInt8 iVal) { return (StringBuilderUTF*)::Text::StringBuilder<T>::AppendHex8(iVal); }
		virtual StringBuilderUTF *AppendHex16(UInt16 iVal) { return (StringBuilderUTF*)::Text::StringBuilder<T>::AppendHex16(iVal); }
		virtual StringBuilderUTF *AppendHex24(UInt32 iVal) { return (StringBuilderUTF*)::Text::StringBuilder<T>::AppendHex24(iVal); }
		virtual StringBuilderUTF *AppendHex32(UInt32 iVal) { return (StringBuilderUTF*)::Text::StringBuilder<T>::AppendHex32(iVal); }
		virtual StringBuilderUTF *AppendHex32V(UInt32 iVal) { return (StringBuilderUTF*)::Text::StringBuilder<T>::AppendHex32V(iVal); }
		virtual StringBuilderUTF *AppendHex64(UInt64 iVal) { return (StringBuilderUTF*)::Text::StringBuilder<T>::AppendHex64(iVal); }
		virtual StringBuilderUTF *AppendHex64V(UInt64 iVal) { return (StringBuilderUTF*)::Text::StringBuilder<T>::AppendHex64V(iVal); }
		virtual StringBuilderUTF *AppendHexOS(UOSInt iVal) { return (StringBuilderUTF*)::Text::StringBuilder<T>::AppendHexOS(iVal); }
		virtual StringBuilderUTF *AppendHexBuff(const UInt8 *buff, UOSInt buffSize, UTF32Char seperator, ::Text::LineBreakType lineBreak) { return (StringBuilderUTF*)::Text::StringBuilder<T>::AppendHex(buff, buffSize, (T)seperator, lineBreak); }
	};
}
#endif
