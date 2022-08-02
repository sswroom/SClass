#ifndef _SM_IO_UNIXCONFIGFILE
#define _SM_IO_UNIXCONFIGFILE
#include "IO/ConfigFile.h"
#include "Text/UTF8Reader.h"

namespace IO
{
	class UnixConfigFile
	{
	public:
		static IO::ConfigFile *Parse(Text::CString fileName);
	private:
		static IO::ConfigFile *ParseReader(Text::UTF8Reader *reader);
	};
}
#endif
