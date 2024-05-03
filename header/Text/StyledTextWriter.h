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

		virtual Bool Write(Text::CStringNN str)
		{
			UOSInt i = 0;
			while (i < str.leng)
			{
				if (!WriteChar(str.v[i])) return false;
				i++;
			}
			return true;
		}

		virtual Bool WriteLine(Text::CStringNN str)
		{
			return Write(str) && WriteChar('\r') && WriteChar('\n');
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
