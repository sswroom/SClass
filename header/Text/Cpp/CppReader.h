#ifndef _SM_TEXT_CPP_CPPREADER
#define _SM_TEXT_CPP_CPPREADER
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"

namespace Text
{
	namespace Cpp
	{
		class CppReader
		{
		private:	
			typedef enum
			{
				ET_NONE,
				ET_MULTILINE_COMMENT,
				ET_STRING
			} EscapeType;
		private:
			Text::UTF8Reader *reader;
			Text::StringBuilderUTF8 *sbLine;
			UOSInt currOfst;
			EscapeType escapeType;

			Bool ReadLineInner(Text::StringBuilderUTF8 *sb);
			Bool ReadWord(Text::StringBuilderUTF *sb, Bool move);
		public:
			CppReader(IO::Stream *stm);
			~CppReader();

			Bool PeekWord(Text::StringBuilderUTF *sb);
			Bool NextWord(Text::StringBuilderUTF *sb);
			Bool ReadLine(Text::StringBuilderUTF8 *sb);
			Bool GetLastLineBreak(Text::StringBuilderUTF *sb);
		};
	}
}
#endif
