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

		virtual StringBuilderUTF *Append(Text::PString *s);
		virtual StringBuilderUTF *Append(const UTF8Char *s);
		virtual StringBuilderUTF *AppendC(const UTF8Char *s, UOSInt charCnt);
		virtual StringBuilderUTF *AppendS(const UTF8Char *s, UOSInt maxLen);
		virtual StringBuilderUTF *AppendChar(UTF32Char c, UOSInt repCnt);
		StringBuilderUTF8 *AppendC2(const UTF8Char *str1, UOSInt len1, const UTF8Char *str2, UOSInt len2);

		StringBuilderUTF8 *AppendCSV(const UTF8Char **sarr, UOSInt nStr);
		StringBuilderUTF8 *AppendToUpper(const UTF8Char *s);
		StringBuilderUTF8 *AppendToLower(const UTF8Char *s);
		StringBuilderUTF8 *RemoveANSIEscapes();
		Bool EqualsC(const UTF8Char *s, UOSInt len);
		Bool EqualsICaseC(const UTF8Char *s, UOSInt len);
		Bool StartsWithC(const UTF8Char *s, UOSInt len);
		UOSInt IndexOfC(const UTF8Char *s, UOSInt len);
		void TrimC();
		void TrimC(UOSInt index);
 	};
}
#endif
