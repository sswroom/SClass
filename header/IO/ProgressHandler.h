#ifndef _SM_IO_PROGRESSHANDLER
#define _SM_IO_PROGRESSHANDLER
#include "Text/CString.h"

namespace IO
{
	class ProgressHandler
	{
	public:
		virtual void ProgressStart(Text::CStringNN name, UInt64 count) = 0;
		virtual void ProgressUpdate(UInt64 currCount, UInt64 newCount) = 0;
		virtual void ProgressEnd() = 0;
	};
}
#endif
