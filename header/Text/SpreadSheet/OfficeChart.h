#ifndef _SM_TEXT_SPREADSHEET_OFFICECHART
#define _SM_TEXT_SPREADSHEET_OFFICECHART
#include "Data/ArrayListNN.h"
#include "Math/Unit/Distance.h"
#include "Text/String.h"
#include "Text/SpreadSheet/OfficeChartAxis.h"
#include "Text/SpreadSheet/OfficeChartSeries.h"
#include "Text/SpreadSheet/OfficeShapeProp.h"
#include "Text/SpreadSheet/WorkbookDataSource.h"

namespace Text
{
	namespace SpreadSheet
	{
		enum class LegendPos
		{
			Bottom
		};

		enum class BlankAs
		{
			Default,
			Gap,
			Zero
		};

		enum class ChartType
		{
			Unknown,
			LineChart
		};

		class OfficeChart
		{
		private:
			static PresetColor seriesColor[];

			Double xInch;
			Double yInch;
			Double wInch;
			Double hInch;
			Optional<Text::String> titleText;
			OfficeShapeProp *shapeProp;
			Bool hasLegend;
			LegendPos legendPos;
			Bool legendOverlay;
			BlankAs displayBlankAs;
			ChartType chartType;
			Optional<OfficeChartAxis> categoryAxis;
			Optional<OfficeChartAxis> valueAxis;
			Data::ArrayListNN<OfficeChartAxis> axes;
			Data::ArrayListNN<OfficeChartSeries> series;

		public:
			OfficeChart(Math::Unit::Distance::DistanceUnit du, Double x, Double y, Double w, Double h);
			~OfficeChart();

			Double GetXInch();
			Double GetYInch();
			Double GetWInch();
			Double GetHInch();

			void SetTitleText(Text::CString titleText);
			Optional<Text::String> GetTitleText();

			OfficeShapeProp *GetShapeProp();
			void SetShapeProp(OfficeShapeProp *shapeProp);
			void AddLegend(LegendPos pos);
			Bool HasLegend();
			LegendPos GetLegendPos();
			Bool IsLegendOverlay();
			void SetDisplayBlankAs(BlankAs displayBlankAs);
			BlankAs GetDisplayBlankAs();

			void InitChart(ChartType chartType, NN<OfficeChartAxis> categoryAxis, NN<OfficeChartAxis> valueAxis);
			void InitLineChart(Text::CString leftAxisName, Text::CString bottomAxisName, AxisType bottomAxisType);
			ChartType GetChartType();
			NN<OfficeChartAxis> CreateAxis(AxisType axisType, AxisPosition axisPos);
			UOSInt GetAxisCount();
			Optional<OfficeChartAxis> GetAxis(UOSInt index);
			UOSInt GetAxisIndex(NN<OfficeChartAxis> axis);
			Optional<OfficeChartAxis> GetCategoryAxis();
			Optional<OfficeChartAxis> GetValueAxis();

			void AddSeries(WorkbookDataSource *categoryData, WorkbookDataSource *valueData, Text::String *name, Bool showMarker);
			void AddSeries(WorkbookDataSource *categoryData, WorkbookDataSource *valueData, Text::CString name, Bool showMarker);
			UOSInt GetSeriesCount() const;
			NN<OfficeChartSeries> GetSeriesNoCheck(UOSInt index) const;
			Optional<OfficeChartSeries> GetSeries(UOSInt index) const;
		};
	}
}
#endif
