#ifndef _SM_IO_WSCONFIGFILE
#define _SM_IO_WSCONFIGFILE
#include "IO/ConfigFile.h"

namespace IO
{
	class WSConfigFile
	{
	public:
		static IO::ConfigFile *Parse(Text::CStringNN fileName);
	};
}
#endif
