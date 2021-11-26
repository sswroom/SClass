#ifndef _SM_TEXT_SPREADSHEET_OFFICESHAPEPROP
#define _SM_TEXT_SPREADSHEET_OFFICESHAPEPROP
#include "Text/SpreadSheet/OfficeLineStyle.h"

namespace Text
{
	namespace SpreadSheet
	{
		class OfficeShapeProp
		{
		private:
			OfficeFill *fill;
			OfficeLineStyle *lineStyle;

		public:
			OfficeShapeProp();
			OfficeShapeProp(OfficeFill *fill);
			OfficeShapeProp(OfficeLineStyle *lineStyle);
			OfficeShapeProp(OfficeFill *fill, OfficeLineStyle *lineStyle);
			~OfficeShapeProp();

			OfficeFill *GetFill();
			void SetFill(OfficeFill *fill);
			OfficeLineStyle *GetLineStyle();
			void SetLineStyle(OfficeLineStyle *lineStyle);
		};
	}
}
#endif
