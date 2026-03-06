#ifndef _SM_TEXT_CSSCOLOR
#define _SM_TEXT_CSSCOLOR
#include "Text/CString.h"

namespace Text
{
	class CSSColor
	{
	public:
		static UInt32 Parse(Text::CStringNN colorStr);
	};
}
#endif
