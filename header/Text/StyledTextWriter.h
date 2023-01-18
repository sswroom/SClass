#ifndef _SM_TEXT_STYLEDTEXTWRITER
#define _SM_TEXT_STYLEDTEXTWRITER
#include "IO/Writer.h"

namespace Text
{
	class StyledTextWriter : public IO::Writer
	{
	public:
		enum class StandardColor
		{
			Black = 0,
			DarkBlue = 1,
			DarkGreen = 2,
			DarkCyan = 3,
			DarkRed = 4,
			DarkMagenta = 5,
			DarkYellow = 6,
			Gray = 7,
			DarkGray = 8,
			Blue = 9,
			Green = 10,
			Cyan = 11,
			Red = 12,
			Magenta = 13,
			Yellow = 14,
			White = 15
		};
	public:
		virtual ~StyledTextWriter() {};

		virtual Bool WriteChar(UTF8Char c) = 0;
		virtual void SetTextColor(StandardColor fgColor) = 0;
		virtual void ResetTextColor() = 0;
	};
}
#endif
