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

		virtual NotNullPtr<StringBuilderUTF> Append(Text::PString *s);
		virtual NotNullPtr<StringBuilderUTF> Append(const UTF8Char *s);
		virtual NotNullPtr<StringBuilderUTF> AppendC(const UTF8Char *s, UOSInt charCnt);
		virtual NotNullPtr<StringBuilderUTF> AppendS(const UTF8Char *s, UOSInt maxLen);
		virtual NotNullPtr<StringBuilderUTF> AppendChar(UTF32Char c, UOSInt repCnt);

		NotNullPtr<StringBuilderUTF16> AppendCSV(const UTF16Char **sarr, UOSInt nStr);
		NotNullPtr<StringBuilderUTF16> AppendToUpper(const UTF16Char *s);
		NotNullPtr<StringBuilderUTF16> AppendToLower(const UTF16Char *s);
 	};
}
#endif
