#ifndef _SM_TEXT_STRINGBUILDERUTF32
#define _SM_TEXT_STRINGBUILDERUTF32
#include "Text/StringBuilder.h"
#include "Text/StringBuilderUTF.h"

namespace Text
{
	class StringBuilderUTF32 : public Text::StringBuilderUTFText<UTF32Char>
	{
	public:
		StringBuilderUTF32();
		virtual ~StringBuilderUTF32();

		virtual NN<StringBuilderUTF> Append(Text::PString *s);
		virtual NN<StringBuilderUTF> Append(const UTF8Char *s);
		virtual NN<StringBuilderUTF> AppendC(const UTF8Char *s, UOSInt charCnt);
		virtual NN<StringBuilderUTF> AppendS(const UTF8Char *s, UOSInt maxLen);
		virtual NN<StringBuilderUTF> AppendChar(UTF32Char c, UOSInt repCnt);

		NN<StringBuilderUTF32> Append(UTF32Char c, UOSInt repeatCnt);
		NN<StringBuilderUTF32> AppendCSV(const UTF32Char **sarr, UOSInt nStr);
		NN<StringBuilderUTF32> AppendToUpper(const UTF32Char *s);
		NN<StringBuilderUTF32> AppendToLower(const UTF32Char *s);
 	};
}
#endif
