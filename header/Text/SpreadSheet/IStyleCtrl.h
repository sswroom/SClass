#ifndef _SM_TEXT_SPREADSHEET_ISTYLECTRL
#define _SM_TEXT_SPREADSHEET_ISTYLECTRL
#include "Text/SpreadSheet/CellStyle.h"

namespace Text
{
	namespace SpreadSheet
	{
		class IStyleCtrl
		{
		public:
			virtual OSInt GetStyleIndex(CellStyle *style) const = 0;
			virtual CellStyle *GetStyle(UOSInt index) const = 0;
			virtual CellStyle *FindOrCreateStyle(const CellStyle *tmpStyle) = 0;
		};
	}
}

#endif
