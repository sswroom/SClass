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

		Bool ReadLineInner(NotNullPtr<Text::StringBuilderUTF8> sb);
		Bool ReadWord(Text::StringBuilderUTF8 *sb, Bool move);
	public:
		MIBReader(NotNullPtr<IO::Stream> stm);
		~MIBReader();

		Bool PeekWord(Text::StringBuilderUTF8 *sb);
		Bool NextWord(Text::StringBuilderUTF8 *sb);
		Bool ReadLine(NotNullPtr<Text::StringBuilderUTF8> sb);
		Bool GetLastLineBreak(NotNullPtr<Text::StringBuilderUTF8> sb);
	};
}
#endif
