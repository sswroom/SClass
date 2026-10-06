#ifndef _SM_IO_YAMLPARSER
#define _SM_IO_YAMLPARSER
#include "IO/ConfigFile.h"
#include "IO/Stream.h"

namespace IO
{
	class YAMLParser
	{
	public:
		static Optional<IO::ConfigFile> ParseStream(NN<IO::Stream> stm);
		static Optional<IO::ConfigFile> ParseFile(Text::CStringNN fileName);
	};
}
#endif
