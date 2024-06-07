#ifndef _SM_IO_INIFILE
#define _SM_IO_INIFILE
#include "IO/ConfigFile.h"
#include "IO/StreamReader.h"
#include "IO/Writer.h"

namespace IO
{
	class IniFile
	{
	public:
		static Optional<IO::ConfigFile> Parse(NN<IO::Stream> stm, UInt32 codePage);
		static Optional<IO::ConfigFile> Parse(Text::CStringNN fileName, UInt32 codePage);
		static Optional<IO::ConfigFile> ParseProgConfig(UInt32 codePage);

	private:
		static Optional<IO::ConfigFile> ParseReader(NN<IO::StreamReader> reader);

	public:
		static Bool SaveConfig(NN<IO::Stream> stm, UInt32 codePage, NN<IO::ConfigFile> cfg);
		static Bool SaveConfig(NN<IO::Writer> writer, NN<IO::ConfigFile> cfg);
	};
}
#endif
