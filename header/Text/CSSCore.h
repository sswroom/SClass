#ifndef _SM_TEXT_CSSCORE
#define _SM_TEXT_CSSCORE
#include "Text/CString.h"

namespace Text
{
	class CSSCore
	{
	public:
		static UInt32 ParseColor(Text::CStringNN colorStr);
		static Double FontSizeToPx(Text::CStringNN fontSizeStr, Double parentFontSizePx);
	};
}
#endif
