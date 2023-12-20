#ifndef _SM_IO_FILESELECTOR
#define _SM_IO_FILESELECTOR
#include "Text/CString.h"

namespace IO
{
	class FileSelector
	{
	public:
		virtual void AddFilter(Text::CStringNN pattern, Text::CStringNN name) = 0;
	};
}
#endif

