#ifndef _SM_IO_INIFILE
#define _SM_IO_INIFILE
#include "IO/ConfigFile.h"
#include "IO/IWriter.h"
#include "IO/StreamReader.h"

namespace IO
{
	class IniFile
	{
	public:
		static IO::ConfigFile *Parse(IO::Stream *stm, Int32 codePage);
		static IO::ConfigFile *Parse(const UTF8Char *fileName, Int32 codePage);
		static IO::ConfigFile *ParseProgConfig(Int32 codePage);

	private:
		static IO::ConfigFile *ParseReader(IO::StreamReader *reader);

	public:
		static Bool SaveConfig(IO::Stream *stm, Int32 codePage, IO::ConfigFile *cfg);
		static Bool SaveConfig(IO::IWriter *writer, IO::ConfigFile *cfg);
	};
};
#endif
