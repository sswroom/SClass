#ifndef _SM_IO_BUILDTIME
#define _SM_IO_BUILDTIME
#include "Data/DateTime.h"
#include "Data/Timestamp.h"

namespace IO
{
	class BuildTime
	{
	public:
		static void GetBuildTime(NN<Data::DateTime> dt);
		static Data::Timestamp GetBuildTime();
	};
};
#endif
