#ifndef _SM_TEXT_STRINGTOOL
#define _SM_TEXT_STRINGTOOL
#include "Data/StringUTF8Map.h"
#include "Text/StringBuilderUTF.h"

namespace Text
{
	class StringTool
	{
	public:
		static void BuildString(Text::StringBuilderUTF *sb, const UTF8Char *s);
		static void BuildString(Text::StringBuilderUTF *sb, Data::StringUTF8Map<const UTF8Char*> *map);
		static void BuildString(Text::StringBuilderUTF *sb, Data::List<const UTF8Char*> *list);
		static Bool IsNonASCII(const UTF8Char *s);
		static Bool IsEmailAddress(const UTF8Char *s);
		static const UTF8Char *Null2Empty(const UTF8Char *s);
	};
}
#endif
