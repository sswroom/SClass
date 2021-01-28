#ifndef _SM_IO_BUILDTIME
#define _SM_IO_BUILDTIME
#include "Data/DateTime.h"

namespace IO
{
	class BuildTime
	{
	public:
		static void GetBuildTime(Data::DateTime *dt);
	};
};
#endif
