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
			virtual OSInt GetStyleIndex(CellStyle *style) = 0;
			virtual CellStyle *GetStyle(OSInt index) = 0;
		};
	};
};

#endif
