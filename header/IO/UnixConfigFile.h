#ifndef _SM_IO_UNIXCONFIGFILE
#define _SM_IO_UNIXCONFIGFILE
#include "IO/ConfigFile.h"
#include "Text/UTF8Reader.h"

namespace IO
{
	class UnixConfigFile
	{
	public:
		static Optional<IO::ConfigFile> Parse(Text::CStringNN fileName);
	private:
		static Optional<IO::ConfigFile> ParseReader(NN<Text::UTF8Reader> reader);
	};
}
#endif
