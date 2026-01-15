#ifndef _SM_TEXT_STRINGBUILDERUTF
#define _SM_TEXT_STRINGBUILDERUTF
#include "Data/DateTime.h"
#include "Text/LineBreakType.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/PString.h"

namespace Text
{
	class StringBuilderUTF
	{
	public:
		virtual void ClearStr() = 0;
		virtual void AllocLeng(UIntOS leng) = 0;
		virtual UIntOS GetCharCnt() = 0;
		virtual Bool EndsWith(UTF8Char c) = 0;
		virtual void RemoveChars(UIntOS cnt) = 0;
		virtual void RemoveChars(UIntOS index, UIntOS cnt) = 0;

		virtual NN<StringBuilderUTF> AppendI16(Int16 iVal) = 0;
		virtual NN<StringBuilderUTF> AppendU16(UInt16 iVal) = 0;
		virtual NN<StringBuilderUTF> AppendI32(Int32 iVal) = 0;
		virtual NN<StringBuilderUTF> AppendU32(UInt32 iVal) = 0;
		virtual NN<StringBuilderUTF> AppendI64(Int64 iVal) = 0;
		virtual NN<StringBuilderUTF> AppendU64(UInt64 iVal) = 0;
		virtual NN<StringBuilderUTF> AppendDate(NN<Data::DateTime> dt) = 0;
		virtual NN<StringBuilderUTF> AppendIntOS(IntOS iVal) = 0;
		virtual NN<StringBuilderUTF> AppendUIntOS(UIntOS iVal) = 0;
		virtual NN<StringBuilderUTF> AppendLB(Text::LineBreakType lbt) = 0;

		virtual NN<StringBuilderUTF> AppendHex8(UInt8 iVal) = 0;
		virtual NN<StringBuilderUTF> AppendHex16(UInt16 iVal) = 0;
		virtual NN<StringBuilderUTF> AppendHex24(UInt32 iVal) = 0;
		virtual NN<StringBuilderUTF> AppendHex32(UInt32 iVal) = 0;
		virtual NN<StringBuilderUTF> AppendHex32V(UInt32 iVal) = 0;
		virtual NN<StringBuilderUTF> AppendHex64(UInt64 iVal) = 0;
		virtual NN<StringBuilderUTF> AppendHex64V(UInt64 iVal) = 0;
		virtual NN<StringBuilderUTF> AppendHexOS(UIntOS iVal) = 0;
		virtual NN<StringBuilderUTF> AppendHexBuff(UnsafeArray<const UInt8> buff, UIntOS buffSize, UTF32Char seperator, Text::LineBreakType lineBreak) = 0;

		virtual NN<StringBuilderUTF> Append(NN<Text::PString> s) = 0;
		virtual NN<StringBuilderUTF> Append(UnsafeArray<const UTF8Char> s) = 0;
		virtual NN<StringBuilderUTF> AppendC(UnsafeArray<const UTF8Char> s, UIntOS charCnt) = 0;
		virtual NN<StringBuilderUTF> AppendS(UnsafeArray<const UTF8Char> s, UIntOS maxLen) = 0;
		virtual NN<StringBuilderUTF> AppendChar(UTF32Char c, UIntOS repCnt) = 0;
	};
}
#endif
