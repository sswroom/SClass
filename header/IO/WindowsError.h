#ifndef _SM_IO_WINDOWSERROR
#define _SM_IO_WINDOWSERROR
#include "Text/CString.h"

namespace IO
{
	class WindowsError
	{
	public:
		static Text::CStringNN GetString(UInt32 errorCode);
	};
}
#endif
