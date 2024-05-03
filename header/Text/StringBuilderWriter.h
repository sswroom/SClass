#ifndef _SM_TEXT_STRINGBUILDERWRITER
#define _SM_TEXT_STRINGBUILDERWRITER
#include "IO/Writer.h"
#include "Text/StringBuilderUTF8.h"

namespace Text
{
	class StringBuilderWriter : public IO::Writer
	{
	private:
		NN<Text::StringBuilderUTF8> sb;
	public:
		StringBuilderWriter(NN<Text::StringBuilderUTF8> sb);
		virtual ~StringBuilderWriter();

		virtual Bool Write(Text::CStringNN str);
		virtual Bool WriteLine(Text::CStringNN str);
		virtual Bool WriteLine();
	};
}
#endif
