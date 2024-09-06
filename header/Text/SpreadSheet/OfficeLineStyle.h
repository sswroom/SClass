#ifndef _SM_TEXT_SPREADSHEET_OFFICELINESTYLE
#define _SM_TEXT_SPREADSHEET_OFFICELINESTYLE
#include "Text/SpreadSheet/OfficeFill.h"

namespace Text
{
	namespace SpreadSheet
	{
		class OfficeLineStyle
		{
		private:
			Optional<OfficeFill> fill;

		public:
			OfficeLineStyle(Optional<OfficeFill> fill);
			~OfficeLineStyle();

			Optional<OfficeFill> GetFillStyle();
		};
	}
}
#endif
