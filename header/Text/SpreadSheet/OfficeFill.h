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
			OfficeColor *color;

			OfficeFill(FillType fillType, OfficeColor *color);
		public:
			~OfficeFill();

			FillType GetFillType();
			OfficeColor *GetColor();

			static OfficeFill *NewSolidFill();
			static OfficeFill *NewSolidFill(OfficeColor *color);
		};
	}
}
#endif
