#ifndef _SM_IO_READER
#define _SM_IO_READER
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	class Reader
	{
	public:
		virtual ~Reader() {}

		virtual void Close() = 0;
		virtual UTF8Char *ReadLine(UTF8Char *buff, UOSInt maxCharCnt) = 0;
		virtual Bool ReadLine(Text::StringBuilderUTF8 *sb, UOSInt maxCharCnt) = 0;
		virtual UTF8Char *GetLastLineBreak(UTF8Char *buff) = 0;
		virtual Bool GetLastLineBreak(Text::StringBuilderUTF8 *sb) = 0;
		virtual Bool ReadToEnd(Text::StringBuilderUTF8 *sb) = 0;
	};
}
#endif
