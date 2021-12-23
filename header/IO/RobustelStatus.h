#ifndef _SM_IO_ROBUSTELSTATUS
#define _SM_IO_ROBUSTELSTATUS
#include "Text/String.h"

namespace IO
{
	class RobustelStatus
	{
	public:
		static Text::String *GetStatus(const Char *name);
	};
}
#endif
