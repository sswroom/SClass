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
			OfficeFill *fill;

		public:
			OfficeLineStyle(OfficeFill *fill);
			~OfficeLineStyle();

			OfficeFill *GetFillStyle();
		};
	}
}
#endif
