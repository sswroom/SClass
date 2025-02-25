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
		virtual UnsafeArrayOpt<UTF8Char> ReadLine(UnsafeArray<UTF8Char> buff, UOSInt maxCharCnt) = 0;
		virtual Bool ReadLine(NN<Text::StringBuilderUTF8> sb, UOSInt maxCharCnt) = 0;
		virtual UnsafeArray<UTF8Char> GetLastLineBreak(UnsafeArray<UTF8Char> buff) = 0;
		virtual Bool GetLastLineBreak(NN<Text::StringBuilderUTF8> sb) = 0;
		virtual Bool IsLineBreak() = 0;
		virtual Bool ReadToEnd(NN<Text::StringBuilderUTF8> sb) = 0;
	};
}
#endif
