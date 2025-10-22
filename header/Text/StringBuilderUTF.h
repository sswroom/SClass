#ifndef _SM_TEXT_STRINGBUILDERUTF
#define _SM_TEXT_STRINGBUILDERUTF
#include "Text/LineBreakType.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/PString.h"
#include "Text/StringBuilder.h"

namespace Text
{
	class StringBuilderUTF
	{
	public:
		virtual void ClearStr() = 0;
		virtual void AllocLeng(UOSInt leng) = 0;
		virtual UOSInt GetCharCnt() = 0;
		virtual Bool EndsWith(UTF8Char c) = 0;
		virtual void RemoveChars(UOSInt cnt) = 0;
		virtual void RemoveChars(UOSInt index, UOSInt cnt) = 0;

		virtual NN<StringBuilderUTF> AppendI16(Int16 iVal) = 0;
		virtual NN<StringBuilderUTF> AppendU16(UInt16 iVal) = 0;
		virtual NN<StringBuilderUTF> AppendI32(Int32 iVal) = 0;
		virtual NN<StringBuilderUTF> AppendU32(UInt32 iVal) = 0;
		virtual NN<StringBuilderUTF> AppendI64(Int64 iVal) = 0;
		virtual NN<StringBuilderUTF> AppendU64(UInt64 iVal) = 0;
		virtual NN<StringBuilderUTF> AppendDate(NN<Data::DateTime> dt) = 0;
		virtual NN<StringBuilderUTF> AppendOSInt(OSInt iVal) = 0;
		virtual NN<StringBuilderUTF> AppendUOSInt(UOSInt iVal) = 0;
		virtual NN<StringBuilderUTF> AppendLB(Text::LineBreakType lbt) = 0;

		virtual NN<StringBuilderUTF> AppendHex8(UInt8 iVal) = 0;
		virtual NN<StringBuilderUTF> AppendHex16(UInt16 iVal) = 0;
		virtual NN<StringBuilderUTF> AppendHex24(UInt32 iVal) = 0;
		virtual NN<StringBuilderUTF> AppendHex32(UInt32 iVal) = 0;
		virtual NN<StringBuilderUTF> AppendHex32V(UInt32 iVal) = 0;
		virtual NN<StringBuilderUTF> AppendHex64(UInt64 iVal) = 0;
		virtual NN<StringBuilderUTF> AppendHex64V(UInt64 iVal) = 0;
		virtual NN<StringBuilderUTF> AppendHexOS(UOSInt iVal) = 0;
		virtual NN<StringBuilderUTF> AppendHexBuff(UnsafeArray<const UInt8> buff, UOSInt buffSize, UTF32Char seperator, Text::LineBreakType lineBreak) = 0;

		virtual NN<StringBuilderUTF> Append(NN<Text::PString> s) = 0;
		virtual NN<StringBuilderUTF> Append(UnsafeArray<const UTF8Char> s) = 0;
		virtual NN<StringBuilderUTF> AppendC(UnsafeArray<const UTF8Char> s, UOSInt charCnt) = 0;
		virtual NN<StringBuilderUTF> AppendS(UnsafeArray<const UTF8Char> s, UOSInt maxLen) = 0;
		virtual NN<StringBuilderUTF> AppendChar(UTF32Char c, UOSInt repCnt) = 0;
	};

	template <class T> class StringBuilderUTFText : public Text::StringBuilder<T>, public Text::StringBuilderUTF
	{
	public:
		virtual void ClearStr() { ::Text::StringBuilder<T>::ClearStr(); }
		virtual void AllocLeng(UOSInt leng) { ::Text::StringBuilder<T>::AllocLeng(leng); }
		virtual UOSInt GetCharCnt() { return this->GetLength(); }
		virtual Bool EndsWith(UTF8Char c) { if (this->buff == this->buffEnd) return false; return this->buffEnd[-1] == (T)c; }
		virtual Bool EndsWith(UnsafeArray<const T> s) { UOSInt l = Text::StrCharCnt(s); if (this->GetLength() < l) return false; return Text::StrEquals((const T*)&this->buffEnd[-(OSInt)l], s); }
		virtual void RemoveChars(UOSInt cnt) {::Text::StringBuilder<T>::RemoveChars(cnt); }
		virtual void RemoveChars(UOSInt index, UOSInt cnt) {::Text::StringBuilder<T>::RemoveChars(index, cnt); }
		virtual NN<StringBuilderUTF> AppendI16(Int16 iVal) { return NN<StringBuilderUTF>::ConvertFrom(::Text::StringBuilder<T>::AppendI16(iVal)); }
		virtual NN<StringBuilderUTF> AppendU16(UInt16 iVal) { return NN<StringBuilderUTF>::ConvertFrom(::Text::StringBuilder<T>::AppendU16(iVal)); }
		virtual NN<StringBuilderUTF> AppendI32(Int32 iVal) { return NN<StringBuilderUTF>::ConvertFrom(::Text::StringBuilder<T>::AppendI32(iVal)); }
		virtual NN<StringBuilderUTF> AppendU32(UInt32 iVal) { return NN<StringBuilderUTF>::ConvertFrom(::Text::StringBuilder<T>::AppendU32(iVal)); }
		virtual NN<StringBuilderUTF> AppendI64(Int64 iVal) { return NN<StringBuilderUTF>::ConvertFrom(::Text::StringBuilder<T>::AppendI64(iVal)); }
		virtual NN<StringBuilderUTF> AppendU64(UInt64 iVal) { return NN<StringBuilderUTF>::ConvertFrom(::Text::StringBuilder<T>::AppendU64(iVal)); }
		virtual NN<StringBuilderUTF> AppendDate(NN<Data::DateTime> dt) { return NN<StringBuilderUTF>::ConvertFrom(::Text::StringBuilder<T>::AppendDate(dt)); }
		virtual NN<StringBuilderUTF> AppendOSInt(OSInt iVal) { return NN<StringBuilderUTF>::ConvertFrom(::Text::StringBuilder<T>::AppendOSInt(iVal)); }
		virtual NN<StringBuilderUTF> AppendUOSInt(UOSInt iVal) { return NN<StringBuilderUTF>::ConvertFrom(::Text::StringBuilder<T>::AppendUOSInt(iVal)); }
		virtual NN<StringBuilderUTF> AppendLB(Text::LineBreakType lbt) {if (lbt == Text::LineBreakType::CRLF) return AppendC(UTF8STRC("\r\n")); else if (lbt == Text::LineBreakType::CR) return AppendChar('\r', 1); else if (lbt == Text::LineBreakType::LF) return AppendChar('\n', 1); else return *this; }

		virtual NN<StringBuilderUTF> AppendHex8(UInt8 iVal) { return NN<StringBuilderUTF>::ConvertFrom(::Text::StringBuilder<T>::AppendHex8(iVal)); }
		virtual NN<StringBuilderUTF> AppendHex16(UInt16 iVal) { return NN<StringBuilderUTF>::ConvertFrom(::Text::StringBuilder<T>::AppendHex16(iVal)); }
		virtual NN<StringBuilderUTF> AppendHex24(UInt32 iVal) { return NN<StringBuilderUTF>::ConvertFrom(::Text::StringBuilder<T>::AppendHex24(iVal)); }
		virtual NN<StringBuilderUTF> AppendHex32(UInt32 iVal) { return NN<StringBuilderUTF>::ConvertFrom(::Text::StringBuilder<T>::AppendHex32(iVal)); }
		virtual NN<StringBuilderUTF> AppendHex32V(UInt32 iVal) { return NN<StringBuilderUTF>::ConvertFrom(::Text::StringBuilder<T>::AppendHex32V(iVal)); }
		virtual NN<StringBuilderUTF> AppendHex64(UInt64 iVal) { return NN<StringBuilderUTF>::ConvertFrom(::Text::StringBuilder<T>::AppendHex64(iVal)); }
		virtual NN<StringBuilderUTF> AppendHex64V(UInt64 iVal) { return NN<StringBuilderUTF>::ConvertFrom(::Text::StringBuilder<T>::AppendHex64V(iVal)); }
		virtual NN<StringBuilderUTF> AppendHexOS(UOSInt iVal) { return NN<StringBuilderUTF>::ConvertFrom(::Text::StringBuilder<T>::AppendHexOS(iVal)); }
		virtual NN<StringBuilderUTF> AppendHexBuff(UnsafeArray<const UInt8> buff, UOSInt buffSize, UTF32Char seperator, ::Text::LineBreakType lineBreak) { return NN<StringBuilderUTF>::ConvertFrom(::Text::StringBuilder<T>::AppendHex(buff, buffSize, (T)seperator, lineBreak)); }
	};
}
#endif
