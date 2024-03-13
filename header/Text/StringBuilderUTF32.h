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

		virtual NotNullPtr<StringBuilderUTF> Append(Text::PString *s);
		virtual NotNullPtr<StringBuilderUTF> Append(const UTF8Char *s);
		virtual NotNullPtr<StringBuilderUTF> AppendC(const UTF8Char *s, UOSInt charCnt);
		virtual NotNullPtr<StringBuilderUTF> AppendS(const UTF8Char *s, UOSInt maxLen);
		virtual NotNullPtr<StringBuilderUTF> AppendChar(UTF32Char c, UOSInt repCnt);

		NotNullPtr<StringBuilderUTF32> Append(UTF32Char c, UOSInt repeatCnt);
		NotNullPtr<StringBuilderUTF32> AppendCSV(const UTF32Char **sarr, UOSInt nStr);
		NotNullPtr<StringBuilderUTF32> AppendToUpper(const UTF32Char *s);
		NotNullPtr<StringBuilderUTF32> AppendToLower(const UTF32Char *s);
 	};
}
#endif
