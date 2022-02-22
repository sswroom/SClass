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
			Text::String *title;
			OfficeShapeProp *shapeProp;
			OfficeShapeProp *majorGridProp;
			TickLabelPosition tickLblPos;
			AxisCrosses crosses;

		public:
			OfficeChartAxis(AxisType axisType, AxisPosition axisPos);
			~OfficeChartAxis();

			AxisType GetAxisType();
			AxisPosition GetAxisPos();
			Text::String *GetTitle();
			void SetTitle(Text::CString title);
			OfficeShapeProp *GetShapeProp();
			void SetShapeProp(OfficeShapeProp *shapeProp);
			OfficeShapeProp *GetMajorGridProp();
			void SetMajorGridProp(OfficeShapeProp *majorGridProp);
			TickLabelPosition GetTickLblPos();
			void SetTickLblPos(TickLabelPosition tickLblPos);
			AxisCrosses GetCrosses();
			void SetCrosses(AxisCrosses axisCrosses);
		};
	}
}
#endif
