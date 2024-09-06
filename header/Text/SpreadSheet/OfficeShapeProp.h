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
			Optional<OfficeFill> fill;
			Optional<OfficeLineStyle> lineStyle;

		public:
			OfficeShapeProp();
			OfficeShapeProp(Optional<OfficeFill> fill);
			OfficeShapeProp(Optional<OfficeLineStyle> lineStyle);
			OfficeShapeProp(Optional<OfficeFill> fill, Optional<OfficeLineStyle> lineStyle);
			~OfficeShapeProp();

			Optional<OfficeFill> GetFill();
			void SetFill(Optional<OfficeFill> fill);
			Optional<OfficeLineStyle> GetLineStyle();
			void SetLineStyle(Optional<OfficeLineStyle> lineStyle);
		};
	}
}
#endif
