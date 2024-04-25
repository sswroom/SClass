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
			virtual OSInt GetStyleIndex(NN<CellStyle> style) const = 0;
			virtual Optional<CellStyle> GetStyle(UOSInt index) const = 0;
			virtual NN<CellStyle> FindOrCreateStyle(NN<const CellStyle> tmpStyle) = 0;
		};
	}
}

#endif
