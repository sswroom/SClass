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

		virtual StringBuilderUTF *Append(Text::PString *s);
		virtual StringBuilderUTF *Append(const UTF8Char *s);
		virtual StringBuilderUTF *AppendC(const UTF8Char *s, UOSInt charCnt);
		virtual StringBuilderUTF *AppendS(const UTF8Char *s, UOSInt maxLen);
		virtual StringBuilderUTF *AppendChar(UTF32Char c, UOSInt repCnt);

		StringBuilderUTF32 *Append(UTF32Char c, UOSInt repeatCnt);
		StringBuilderUTF32 *AppendCSV(const UTF32Char **sarr, UOSInt nStr);
		StringBuilderUTF32 *AppendToUpper(const UTF32Char *s);
		StringBuilderUTF32 *AppendToLower(const UTF32Char *s);
 	};
}
#endif
