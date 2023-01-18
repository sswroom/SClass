#ifndef _SM_TEXT_STYLEDTEXTWRITER
#define _SM_TEXT_STYLEDTEXTWRITER
#include "IO/Writer.h"
#include "Text/StandardColor.h"

namespace Text
{
	class StyledTextWriter : public IO::Writer
	{
	public:
		virtual ~StyledTextWriter() {};

		virtual Bool WriteChar(UTF8Char c) = 0;
		virtual void SetTextColor(StandardColor fgColor) = 0;
		virtual void ResetTextColor() = 0;
	};
}
#endif
