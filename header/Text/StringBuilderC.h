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

		StringBuilderC *Append(const Char *s);
		StringBuilderC *Append(const Char *s, UOSInt charCnt);
		StringBuilderC *AppendChar(Char c, UOSInt repeatCnt);
		StringBuilderC *AppendCSV(const Char **sarr, UOSInt nStr);
		StringBuilderC *AppendToUpper(const Char *s);
		StringBuilderC *AppendToLower(const Char *s);
		StringBuilderC *AppendASCII(const Char *s);
		StringBuilderC *AppendASCII(const Char *s, UOSInt charCnt);
	};
}
#endif
