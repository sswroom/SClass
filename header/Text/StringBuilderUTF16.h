#ifndef _SM_TEXT_STRINGBUILDERUTF16
#define _SM_TEXT_STRINGBUILDERUTF16
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilder.h"
#include "Text/StringBuilderUTF.h"

namespace Text
{
	class StringBuilderUTF16 : public Text::StringBuilderUTFText<UTF16Char>
	{
	public:
		StringBuilderUTF16();
		virtual ~StringBuilderUTF16();

		virtual NN<StringBuilderUTF> Append(Text::PString *s);
		virtual NN<StringBuilderUTF> Append(UnsafeArray<const UTF8Char> s);
		virtual NN<StringBuilderUTF> AppendC(UnsafeArray<const UTF8Char> s, UOSInt charCnt);
		virtual NN<StringBuilderUTF> AppendS(UnsafeArray<const UTF8Char> s, UOSInt maxLen);
		virtual NN<StringBuilderUTF> AppendChar(UTF32Char c, UOSInt repCnt);

		NN<StringBuilderUTF16> AppendCSV(const UTF16Char **sarr, UOSInt nStr);
		NN<StringBuilderUTF16> AppendToUpper(const UTF16Char *s);
		NN<StringBuilderUTF16> AppendToLower(const UTF16Char *s);
 	};
}
#endif
