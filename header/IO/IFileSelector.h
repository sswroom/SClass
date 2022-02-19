#ifndef _SM_IO_IFILESELECTOR
#define _SM_IO_IFILESELECTOR
#include "Text/CString.h"

namespace IO
{
	class IFileSelector
	{
	public:
		virtual void AddFilter(Text::CString pattern, Text::CString name) = 0;
	};
};
#endif

