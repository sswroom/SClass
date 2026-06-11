#ifndef _SM_IO_ROBUSTELSTATUS
#define _SM_IO_ROBUSTELSTATUS
#include "Text/String.h"

namespace IO
{
	class RobustelStatus
	{
	public:
		static Optional<Text::String> GetStatus(UnsafeArray<const Char> name);
	};
}
#endif
