#ifndef _SM_TEXT_STRINGBUILDERC
#define _SM_TEXT_STRINGBUILDERC
#include "Text/StringBuilder.h"

namespace Text
{
	class StringBuilderC : public Text::StringBuilder<Char>
	{
	public:
		StringBuilderC();
		virtual ~StringBuilderC();

		NN<StringBuilderC> Append(UnsafeArray<const Char> s);
		NN<StringBuilderC> AppendC(UnsafeArray<const Char> s, UOSInt charCnt);
		NN<StringBuilderC> AppendChar(Char c, UOSInt repeatCnt);
		NN<StringBuilderC> AppendCSV(UnsafeArray<UnsafeArray<const Char>> sarr, UOSInt nStr);
		NN<StringBuilderC> AppendToUpper(UnsafeArray<const Char> s);
		NN<StringBuilderC> AppendToLower(UnsafeArray<const Char> s);
		NN<StringBuilderC> AppendASCII(UnsafeArray<const Char> s);
		NN<StringBuilderC> AppendASCII(UnsafeArray<const Char> s, UOSInt charCnt);
	};
}
#endif
