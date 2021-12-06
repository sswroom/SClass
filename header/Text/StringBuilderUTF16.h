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

		virtual StringBuilderUTF *Append(Text::String *s);
		virtual StringBuilderUTF *Append(const UTF8Char *s);
		virtual StringBuilderUTF *AppendC(const UTF8Char *s, UOSInt charCnt);
		virtual StringBuilderUTF *AppendS(const UTF8Char *s, UOSInt maxLen);
		virtual StringBuilderUTF *AppendChar(UTF32Char c, UOSInt repCnt);

		StringBuilderUTF16 *AppendCSV(const UTF16Char **sarr, UOSInt nStr);
		StringBuilderUTF16 *AppendToUpper(const UTF16Char *s);
		StringBuilderUTF16 *AppendToLower(const UTF16Char *s);
 	};
}
#endif
