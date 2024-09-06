#ifndef _SM_TEXT_SPREADSHEET_OFFICECHARTAXIS
#define _SM_TEXT_SPREADSHEET_OFFICECHARTAXIS
#include "Text/String.h"
#include "Text/SpreadSheet/OfficeShapeProp.h"

namespace Text
{
	namespace SpreadSheet
	{
		enum class AxisType
		{
			Date,
			Category,
			Numeric,
			Series
		};

		enum class AxisPosition
		{
			Left,
			Top,
			Right,
			Bottom
		};

		enum class TickLabelPosition
		{
			High,
			Low,
			NextTo,
			None
		};

		enum class AxisCrosses
		{
			AutoZero,
			Max,
			Min
		};

		class OfficeChartAxis
		{
		private:
			AxisType axisType;
			AxisPosition axisPos;
			Optional<Text::String> title;
			Optional<OfficeShapeProp> shapeProp;
			Optional<OfficeShapeProp> majorGridProp;
			TickLabelPosition tickLblPos;
			AxisCrosses crosses;

		public:
			OfficeChartAxis(AxisType axisType, AxisPosition axisPos);
			~OfficeChartAxis();

			AxisType GetAxisType();
			AxisPosition GetAxisPos();
			Optional<Text::String> GetTitle();
			void SetTitle(Text::CString title);
			Optional<OfficeShapeProp> GetShapeProp();
			void SetShapeProp(Optional<OfficeShapeProp> shapeProp);
			Optional<OfficeShapeProp> GetMajorGridProp();
			void SetMajorGridProp(Optional<OfficeShapeProp> majorGridProp);
			TickLabelPosition GetTickLblPos();
			void SetTickLblPos(TickLabelPosition tickLblPos);
			AxisCrosses GetCrosses();
			void SetCrosses(AxisCrosses axisCrosses);
		};
	}
}
#endif
