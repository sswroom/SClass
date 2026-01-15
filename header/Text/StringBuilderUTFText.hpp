#ifndef _SM_TEXT_STRINGBUILDERUTFTEXT
#define _SM_TEXT_STRINGBUILDERUTFTEXT
#include "Text/StringBuilder.hpp"
#include "Text/StringBuilderUTF.h"

namespace Text
{

	template <class T> class StringBuilderUTFText : public Text::StringBuilder<T>, public Text::StringBuilderUTF
	{
	public:
		virtual void ClearStr() { ::Text::StringBuilder<T>::ClearStr(); }
		virtual void AllocLeng(UIntOS leng) { ::Text::StringBuilder<T>::AllocLeng(leng); }
		virtual UIntOS GetCharCnt() { return this->GetLength(); }
		virtual Bool EndsWith(UTF8Char c) { if (this->buff == this->buffEnd) return false; return this->buffEnd[-1] == (T)c; }
		virtual Bool EndsWith(UnsafeArray<const T> s) { UIntOS l = Text::StrCharCnt(s); if (this->GetLength() < l) return false; return Text::StrEquals((const T*)&this->buffEnd[-(IntOS)l], s); }
		virtual void RemoveChars(UIntOS cnt) {::Text::StringBuilder<T>::RemoveChars(cnt); }
		virtual void RemoveChars(UIntOS index, UIntOS cnt) {::Text::StringBuilder<T>::RemoveChars(index, cnt); }
		virtual NN<StringBuilderUTF> AppendI16(Int16 iVal) { return NN<StringBuilderUTF>::ConvertFrom(::Text::StringBuilder<T>::AppendI16(iVal)); }
		virtual NN<StringBuilderUTF> AppendU16(UInt16 iVal) { return NN<StringBuilderUTF>::ConvertFrom(::Text::StringBuilder<T>::AppendU16(iVal)); }
		virtual NN<StringBuilderUTF> AppendI32(Int32 iVal) { return NN<StringBuilderUTF>::ConvertFrom(::Text::StringBuilder<T>::AppendI32(iVal)); }
		virtual NN<StringBuilderUTF> AppendU32(UInt32 iVal) { return NN<StringBuilderUTF>::ConvertFrom(::Text::StringBuilder<T>::AppendU32(iVal)); }
		virtual NN<StringBuilderUTF> AppendI64(Int64 iVal) { return NN<StringBuilderUTF>::ConvertFrom(::Text::StringBuilder<T>::AppendI64(iVal)); }
		virtual NN<StringBuilderUTF> AppendU64(UInt64 iVal) { return NN<StringBuilderUTF>::ConvertFrom(::Text::StringBuilder<T>::AppendU64(iVal)); }
		virtual NN<StringBuilderUTF> AppendDate(NN<Data::DateTime> dt) { return NN<StringBuilderUTF>::ConvertFrom(::Text::StringBuilder<T>::AppendDate(dt)); }
		virtual NN<StringBuilderUTF> AppendIntOS(IntOS iVal) { return NN<StringBuilderUTF>::ConvertFrom(::Text::StringBuilder<T>::AppendIntOS(iVal)); }
		virtual NN<StringBuilderUTF> AppendUIntOS(UIntOS iVal) { return NN<StringBuilderUTF>::ConvertFrom(::Text::StringBuilder<T>::AppendUIntOS(iVal)); }
		virtual NN<StringBuilderUTF> AppendLB(Text::LineBreakType lbt) {if (lbt == Text::LineBreakType::CRLF) return AppendC(UTF8STRC("\r\n")); else if (lbt == Text::LineBreakType::CR) return AppendChar('\r', 1); else if (lbt == Text::LineBreakType::LF) return AppendChar('\n', 1); else return NNTHIS; }

		virtual NN<StringBuilderUTF> AppendHex8(UInt8 iVal) { return NN<StringBuilderUTF>::ConvertFrom(::Text::StringBuilder<T>::AppendHex8(iVal)); }
		virtual NN<StringBuilderUTF> AppendHex16(UInt16 iVal) { return NN<StringBuilderUTF>::ConvertFrom(::Text::StringBuilder<T>::AppendHex16(iVal)); }
		virtual NN<StringBuilderUTF> AppendHex24(UInt32 iVal) { return NN<StringBuilderUTF>::ConvertFrom(::Text::StringBuilder<T>::AppendHex24(iVal)); }
		virtual NN<StringBuilderUTF> AppendHex32(UInt32 iVal) { return NN<StringBuilderUTF>::ConvertFrom(::Text::StringBuilder<T>::AppendHex32(iVal)); }
		virtual NN<StringBuilderUTF> AppendHex32V(UInt32 iVal) { return NN<StringBuilderUTF>::ConvertFrom(::Text::StringBuilder<T>::AppendHex32V(iVal)); }
		virtual NN<StringBuilderUTF> AppendHex64(UInt64 iVal) { return NN<StringBuilderUTF>::ConvertFrom(::Text::StringBuilder<T>::AppendHex64(iVal)); }
		virtual NN<StringBuilderUTF> AppendHex64V(UInt64 iVal) { return NN<StringBuilderUTF>::ConvertFrom(::Text::StringBuilder<T>::AppendHex64V(iVal)); }
		virtual NN<StringBuilderUTF> AppendHexOS(UIntOS iVal) { return NN<StringBuilderUTF>::ConvertFrom(::Text::StringBuilder<T>::AppendHexOS(iVal)); }
		virtual NN<StringBuilderUTF> AppendHexBuff(UnsafeArray<const UInt8> buff, UIntOS buffSize, UTF32Char seperator, ::Text::LineBreakType lineBreak) { return NN<StringBuilderUTF>::ConvertFrom(::Text::StringBuilder<T>::AppendHex(buff, buffSize, (T)seperator, lineBreak)); }
	};
}
#endif
