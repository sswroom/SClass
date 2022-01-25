#ifndef _SM_TEXT_STRINGBUILDERWRITER
#define _SM_TEXT_STRINGBUILDERWRITER
#include "IO/Writer.h"
#include "Text/StringBuilderUTF8.h"

namespace Text
{
	class StringBuilderWriter : public IO::Writer
	{
	private:
		Text::StringBuilderUTF8 *sb;
	public:
		StringBuilderWriter(Text::StringBuilderUTF8 *sb);
		virtual ~StringBuilderWriter();

		virtual Bool WriteStrC(const UTF8Char *str, UOSInt nChar);
		virtual Bool WriteStr(const UTF8Char *str);
		virtual Bool WriteLineC(const UTF8Char *str, UOSInt nChar);
		virtual Bool WriteLine(const UTF8Char *str);
		virtual Bool WriteLine();
	};
}
#endif
