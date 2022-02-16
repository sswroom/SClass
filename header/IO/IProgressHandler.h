#ifndef _SM_IO_IPROGRESSHANDLER
#define _SM_IO_IPROGRESSHANDLER
#include "Text/CString.h"

namespace IO
{
	class IProgressHandler
	{
	public:
		virtual void ProgressStart(Text::CString name, UInt64 count) = 0;
		virtual void ProgressUpdate(UInt64 currCount, UInt64 newCount) = 0;
		virtual void ProgressEnd() = 0;
	};
}
#endif
