#ifndef _SM_TEXT_STRINGBUILDERWRITER
#define _SM_TEXT_STRINGBUILDERWRITER
#include "IO/IWriter.h"
#include "Text/StringBuilderUTF.h"

namespace Text
{
	class StringBuilderWriter : public IO::IWriter
	{
	private:
		Text::StringBuilderUTF *sb;
	public:
		StringBuilderWriter(Text::StringBuilderUTF *sb);
		virtual ~StringBuilderWriter();

		virtual Bool Write(const UTF8Char *str, UOSInt nChar);
		virtual Bool Write(const UTF8Char *str);
		virtual Bool WriteLine(const UTF8Char *str, UOSInt nChar);
		virtual Bool WriteLine(const UTF8Char *str);
		virtual Bool WriteLine();
	};
}
#endif
