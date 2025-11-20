#ifndef _SM_TEXT_STRINGBUILDERUTF32
#define _SM_TEXT_STRINGBUILDERUTF32
#include "Text/StringBuilder.hpp"
#include "Text/StringBuilderUTFText.hpp"

namespace Text
{
	class StringBuilderUTF32 : public Text::StringBuilderUTFText<UTF32Char>
	{
	public:
		StringBuilderUTF32();
		virtual ~StringBuilderUTF32();

		virtual NN<StringBuilderUTF> Append(NN<Text::PString> s);
		virtual NN<StringBuilderUTF> Append(UnsafeArray<const UTF8Char> s);
		virtual NN<StringBuilderUTF> AppendC(UnsafeArray<const UTF8Char> s, UOSInt charCnt);
		virtual NN<StringBuilderUTF> AppendS(UnsafeArray<const UTF8Char> s, UOSInt maxLen);
		virtual NN<StringBuilderUTF> AppendChar(UTF32Char c, UOSInt repCnt);

		NN<StringBuilderUTF32> Append(UTF32Char c, UOSInt repeatCnt);
		NN<StringBuilderUTF32> AppendCSV(UnsafeArray<UnsafeArray<const UTF32Char>> sarr, UOSInt nStr);
		NN<StringBuilderUTF32> AppendToUpper(UnsafeArray<const UTF32Char> s);
		NN<StringBuilderUTF32> AppendToLower(UnsafeArray<const UTF32Char> s);
 	};
}
#endif
