#ifndef _SM_TEXT_SPREADSHEET_OFFICECHART
#define _SM_TEXT_SPREADSHEET_OFFICECHART
#include "Math/Unit/Distance.h"

namespace Text
{
	namespace SpreadSheet
	{
		class OfficeChart
		{
		private:
			Double xInch;
			Double yInch;
			Double wInch;
			Double hInch;
			const UTF8Char *titleText;

		public:
			OfficeChart(Math::Unit::Distance::DistanceUnit du, Double x, Double y, Double w, Double h);
			~OfficeChart();

			Double GetXInch();
			Double GetYInch();
			Double GetWInch();
			Double GetHInch();

			void SetTitleText(const UTF8Char *titleText);
			const UTF8Char *GetTitleText();
		};
	}
}
#endif
