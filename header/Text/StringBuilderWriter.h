#ifndef _SM_TEXT_STRINGBUILDERWRITER
#define _SM_TEXT_STRINGBUILDERWRITER
#include "IO/Writer.h"
#include "Text/StringBuilderUTF8.h"

namespace Text
{
	class StringBuilderWriter : public IO::Writer
	{
	private:
		NotNullPtr<Text::StringBuilderUTF8> sb;
	public:
		StringBuilderWriter(NotNullPtr<Text::StringBuilderUTF8> sb);
		virtual ~StringBuilderWriter();

		virtual Bool WriteStrC(const UTF8Char *str, UOSInt nChar);
		virtual Bool WriteLineC(const UTF8Char *str, UOSInt nChar);
		virtual Bool WriteLine();
	};
}
#endif
