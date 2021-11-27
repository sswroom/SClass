#ifndef _SM_TEXT_SPREADSHEET_OFFICECHARTSERIES
#define _SM_TEXT_SPREADSHEET_OFFICECHARTSERIES
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
			WorkbookDataSource *categoryData;
			WorkbookDataSource *valueData;
			const UTF8Char *title;
			Bool smooth;
			OfficeShapeProp *shapeProp;
			UInt32 markerSize;
			MarkerStyle markerStyle;

		public:
			OfficeChartSeries(WorkbookDataSource *categoryData, WorkbookDataSource *valueData);
			~OfficeChartSeries();

			WorkbookDataSource *GetCategoryData();
			WorkbookDataSource *GetValueData();
			const UTF8Char *GetTitle();
			void SetTitle(const UTF8Char *title, const UTF8Char *dataSource);
			Bool IsSmooth();
			void SetSmooth(Bool smooth);
			OfficeShapeProp *GetShapeProp();
			void SetShapeProp(OfficeShapeProp *shapeProp);
			void SetLineStyle(OfficeLineStyle *lineStyle);
			UInt32 GetMarkerSize();
			void SetMarkerSize(UInt32 markerSize);
			MarkerStyle GetMarkerStyle();
			void SetMarkerStyle(MarkerStyle markerStyle);
		};
	}
}
#endif
