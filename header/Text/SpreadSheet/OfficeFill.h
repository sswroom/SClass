#ifndef _SM_TEXT_SPREADSHEET_OFFICEFILL
#define _SM_TEXT_SPREADSHEET_OFFICEFILL
#include "Text/SpreadSheet/OfficeColor.h"

namespace Text
{
	namespace SpreadSheet
	{
		enum class FillType
		{
			SolidFill
		};

		class OfficeFill
		{
		private:
			FillType fillType;
			Optional<OfficeColor> color;

			OfficeFill(FillType fillType, Optional<OfficeColor> color);
		public:
			~OfficeFill();

			FillType GetFillType();
			Optional<OfficeColor> GetColor();

			static NN<OfficeFill> NewSolidFill();
			static NN<OfficeFill> NewSolidFill(Optional<OfficeColor> color);
		};
	}
}
#endif
