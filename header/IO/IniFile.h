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
		static IO::ConfigFile *Parse(IO::Stream *stm, UInt32 codePage);
		static IO::ConfigFile *Parse(const UTF8Char *fileName, UInt32 codePage);
		static IO::ConfigFile *ParseProgConfig(UInt32 codePage);

	private:
		static IO::ConfigFile *ParseReader(IO::StreamReader *reader);

	public:
		static Bool SaveConfig(IO::Stream *stm, UInt32 codePage, IO::ConfigFile *cfg);
		static Bool SaveConfig(IO::Writer *writer, IO::ConfigFile *cfg);
	};
}
#endif
