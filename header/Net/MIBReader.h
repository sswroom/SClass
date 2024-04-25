#ifndef _SM_NET_MIBREADER
#define _SM_NET_MIBREADER
#include "Net/MIBReader.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"

namespace Net
{
	class MIBReader
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
		MIBReader(NN<IO::Stream> stm);
		~MIBReader();

		Bool PeekWord(NN<Text::StringBuilderUTF8> sb);
		Bool NextWord(NN<Text::StringBuilderUTF8> sb);
		Bool ReadLine(NN<Text::StringBuilderUTF8> sb);
		Bool GetLastLineBreak(NN<Text::StringBuilderUTF8> sb);
	};
}
#endif
