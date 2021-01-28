#ifndef _SM_TEXT_STRINGBUILDERUTF8
#define _SM_TEXT_STRINGBUILDERUTF8
#include "Text/StringBuilder.h"
#include "Text/StringBuilderUTF.h"

namespace Text
{
	class StringBuilderUTF8 : public Text::StringBuilderUTFText<UTF8Char>
	{
	public:
		StringBuilderUTF8();
		virtual ~StringBuilderUTF8();

		virtual StringBuilderUTF *Append(const UTF8Char *s);
		virtual StringBuilderUTF *AppendC(const UTF8Char *s, UOSInt charCnt);
		virtual StringBuilderUTF *AppendS(const UTF8Char *s, UOSInt maxLen);
		virtual StringBuilderUTF *AppendChar(UTF32Char c, UOSInt repCnt);

		StringBuilderUTF8 *AppendCSV(const UTF8Char **sarr, UOSInt nStr);
		StringBuilderUTF8 *AppendToUpper(const UTF8Char *s);
		StringBuilderUTF8 *AppendToLower(const UTF8Char *s);
 	};
}
#endif
