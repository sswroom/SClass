#ifndef _SM_MANAGE_HANDLETYPE
#define _SM_MANAGE_HANDLETYPE
#include "Text/CString.h"

namespace Manage
{
	enum class HandleType
	{
		File,
		Device,
		EPoll,
		Event,
		INotify,
		Signal,
		Timer,
		Socket,
		Pipe,
		Memory
	};

	Text::CString HandleTypeGetName(HandleType handleType);
}
#endif
