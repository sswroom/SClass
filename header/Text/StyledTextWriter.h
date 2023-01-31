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

		virtual Bool WriteStrC(const UTF8Char *str, UOSInt nChar)
		{
			UOSInt i = 0;
			while (i < nChar)
			{
				if (!WriteChar(str[i])) return false;
				i++;
			}
			return true;
		}

		virtual Bool WriteLineC(const UTF8Char *str, UOSInt nChar)
		{
			return WriteStrC(str, nChar) && WriteChar('\r') && WriteChar('\n');
		}

		virtual Bool WriteLine()
		{
			return WriteChar('\r') && WriteChar('\n');
		}

		virtual Bool WriteChar(UTF8Char c) = 0;
		virtual void SetTextColor(StandardColor fgColor) = 0;
		virtual void ResetTextColor() = 0;
	};
}
#endif
