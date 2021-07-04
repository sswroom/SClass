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
		Text::UTF8Reader *reader;
		Text::StringBuilderUTF8 *sbLine;
		UOSInt currOfst;
		Bool multiLineComment;

		Bool ReadLineInner(Text::StringBuilderUTF8 *sb);
		Bool ReadWord(Text::StringBuilderUTF *sb, Bool move);
	public:
		MIBReader(IO::Stream *stm);
		~MIBReader();

		Bool PeekWord(Text::StringBuilderUTF *sb);
		Bool NextWord(Text::StringBuilderUTF *sb);
		Bool ReadLine(Text::StringBuilderUTF8 *sb);
		Bool GetLastLineBreak(Text::StringBuilderUTF *sb);
	};
}
#endif
