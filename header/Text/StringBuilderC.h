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
		StringBuilderC *Append(const Char *s, OSInt charCnt);
		StringBuilderC *Append(Char c, OSInt repeatCnt);
		StringBuilderC *AppendCSV(const Char **sarr, OSInt nStr);
		StringBuilderC *AppendToUpper(const Char *s);
		StringBuilderC *AppendToLower(const Char *s);
		StringBuilderC *AppendASCII(Char *s);
		StringBuilderC *AppendASCII(Char *s, OSInt charCnt);
	};
};
#endif
