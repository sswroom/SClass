#ifndef _SM_TEXT_SPREADSHEET_OFFICECHARTSERIES
#define _SM_TEXT_SPREADSHEET_OFFICECHARTSERIES
#include "Text/String.h"
#include "Text/SpreadSheet/OfficeShapeProp.h"
#include "Text/SpreadSheet/WorkbookDataSource.h"

namespace Text
{
	namespace SpreadSheet
	{
		enum class MarkerStyle
		{
			Circle,
			Dash,
			Diamond,
			Dot,
			None,
			Picture,
			Plus,
			Square,
			Star,
			Triangle,
			X
		};

		class OfficeChartSeries
		{
		private:
			NN<WorkbookDataSource> categoryData;
			NN<WorkbookDataSource> valueData;
			Optional<Text::String> title;
			Bool smooth;
			Optional<OfficeShapeProp> shapeProp;
			UInt32 markerSize;
			MarkerStyle markerStyle;

		public:
			OfficeChartSeries(NN<WorkbookDataSource> categoryData, NN<WorkbookDataSource> valueData);
			~OfficeChartSeries();

			NN<WorkbookDataSource> GetCategoryData();
			NN<WorkbookDataSource> GetValueData();
			Optional<Text::String> GetTitle();
			void SetTitle(Optional<Text::String> title, Text::String *dataSource);
			void SetTitle(Text::CString title, const UTF8Char *dataSource);
			Bool IsSmooth();
			void SetSmooth(Bool smooth);
			Optional<OfficeShapeProp> GetShapeProp();
			void SetShapeProp(Optional<OfficeShapeProp> shapeProp);
			void SetLineStyle(Optional<OfficeLineStyle> lineStyle);
			UInt32 GetMarkerSize();
			void SetMarkerSize(UInt32 markerSize);
			MarkerStyle GetMarkerStyle();
			void SetMarkerStyle(MarkerStyle markerStyle);
		};
	}
}
#endif
