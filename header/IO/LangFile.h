#ifndef _SM_IO_LANGFILE
#define _SM_IO_LANGFILE
#include "IO/ConfigFile.h"

namespace IO
{
	class LangFile
	{
	private:
		Optional<IO::ConfigFile> cfg;

	public:
		LangFile(Text::CStringNN fileName, UInt32 codePage);
		virtual ~LangFile();

		Text::CStringNN GetValue(Text::CStringNN name);
		Text::CStringNN GetValue(Text::CStringNN category, Text::CStringNN name);
	};
}
#endif
