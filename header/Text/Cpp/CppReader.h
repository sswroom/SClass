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
			Text::UTF8Reader reader;
			Text::StringBuilderUTF8 sbLine;
			UOSInt currOfst;
			EscapeType escapeType;

			Bool ReadLineInner(NN<Text::StringBuilderUTF8> sb);
			Bool ReadWord(NN<Text::StringBuilderUTF8> sb, Bool move);
		public:
			CppReader(NN<IO::Stream> stm);
			~CppReader();

			Bool PeekWord(NN<Text::StringBuilderUTF8> sb);
			Bool NextWord(NN<Text::StringBuilderUTF8> sb);
			Bool ReadLine(NN<Text::StringBuilderUTF8> sb);
			Bool GetLastLineBreak(NN<Text::StringBuilderUTF8> sb);
		};
	}
}
#endif
