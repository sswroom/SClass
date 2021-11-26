#ifndef _SM_TEXT_SPREADSHEET_OFFICECHART
#define _SM_TEXT_SPREADSHEET_OFFICECHART
#include "Math/Unit/Distance.h"
#include "Text/SpreadSheet/OfficeShapeProp.h"

namespace Text
{
	namespace SpreadSheet
	{
		enum class LegendPos
		{
			Bottom
		};

		class OfficeChart
		{
		private:
			Double xInch;
			Double yInch;
			Double wInch;
			Double hInch;
			const UTF8Char *titleText;
			OfficeShapeProp *shapeProp;
			Bool hasLegend;
			LegendPos legendPos;
			Bool legendOverlay;

		public:
			OfficeChart(Math::Unit::Distance::DistanceUnit du, Double x, Double y, Double w, Double h);
			~OfficeChart();

			Double GetXInch();
			Double GetYInch();
			Double GetWInch();
			Double GetHInch();

			void SetTitleText(const UTF8Char *titleText);
			const UTF8Char *GetTitleText();

			OfficeShapeProp *GetShapeProp();
			void SetShapeProp(OfficeShapeProp *shapeProp);
			void AddLegend(LegendPos pos);
			Bool HasLegend();
			LegendPos GetLegendPos();
			Bool IsLegendOverlay();
		};
	}
}
#endif
