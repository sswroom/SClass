#ifndef _SM_IO_WSCONFIGFILE
#define _SM_IO_WSCONFIGFILE
#include "IO/ConfigFile.h"

namespace IO
{
	class WSConfigFile
	{
	public:
		static IO::ConfigFile *Parse(const UTF8Char *fileName);
	};
}
#endif
