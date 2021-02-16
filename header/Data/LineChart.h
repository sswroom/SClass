#ifndef _SM_DATA_LINECHART
#define _SM_DATA_LINECHART
#include "Data/ArrayList.h"
#include "Data/ArrayListInt32.h"
#include "Data/DateTime.h"
#include "Data/IChart.h"
#include "Data/RandomOS.h"

namespace Data
{
	
	class LineChart : public Data::IChart
	{
	private:
		typedef enum
		{
			CT_NO = 0,
			CT_DATETICK = 1,
			CT_DOUBLE = 2,
			CT_INTEGER = 3
		} ChartType;

	public:
		typedef enum
		{
			PT_NULL = 0,
			PT_CIRCLE = 1
		} PointType;

		typedef enum
		{
			LS_LINE = 0,
			LS_FILL = 1
		} LineStyle;

		typedef enum
		{
			RT_NONE = 0,
			RT_LEFTALIGN = 1,
			RT_RIGHTALIGN = 2
		} RefType;

	private:
		class ChartData
		{
		public:
			void *data;
			Data::LineChart::ChartType dataType;
			Int32 lineColor;
			UOSInt dataCnt;
			Data::LineChart::LineStyle lineStyle;

			ChartData(void *data, UOSInt dataCnt, Data::LineChart::ChartType dataType, Int32 lineColor, Data::LineChart::LineStyle lineStyle);
			~ChartData();
		};

	private:
		Data::RandomOS *rnd;
		Data::ArrayList<void*> *xDatas;
		Data::ArrayList<UOSInt> *xDataCnt;
		ChartType xType;
		const UTF8Char *xLabel;

		Data::ArrayList<ChartData*> *yCharts;
		Data::ArrayList<const UTF8Char*> *dataNames;
//		Data::ArrayList *dataPos;
		const UTF8Char *yLabel;
		const UTF8Char *title;
		UTF8Char *titleLine[3];
		UOSInt titleLineCnt;
		const UTF8Char *yUnit;
		Int32 timeZoneQHR;

		Double refDbl;
		Int32 refInt;
		Int64 refTime;
		Bool refExist;
		RefType refType;
//		LineStyle style;
		PointType pointType;
		Double pointSize;

		Int32 bgColor;
		Int32 boundColor;
		Int32 fontColor;
		Int32 gridColor;
		Int32 refLineColor;
		Double lineThick;
		Double barLength;

		const UTF8Char *fntName;
		Double fntSizePt;

		const Char *dateFormat;
		const Char *timeFormat;
		const Char *dblFormat;
		Double minDblVal;

		Bool hasXRangeDate;
		Int64 xRangeDateMin;
		Int64 xRangeDateMax;
		Bool hasYRangeInt;
		Int32 yRangeIntMin;
		Int32 yRangeIntMax;
		Bool hasYRangeDbl;
		Double yRangeDblMin;
		Double yRangeDblMax;

	public:
		LineChart(const UTF8Char *title);
		virtual ~LineChart();
		void SetTitle(const UTF8Char *title);
		Bool AddXData(Data::DateTime **data, UOSInt dataCnt);
		Bool AddXData(Double *data, UOSInt dataCnt);
		Bool AddXData(Int32 *data, UOSInt dataCnt);
		Bool AddXDataDate(Int64 *data, UOSInt dataCnt);
		void SetXAxis(const UTF8Char *name);
		void SetDateFormat(const Char *format);
		void SetTimeFormat(const Char *format);
		void SetDblFormat(const Char *format);
		void SetFontHeightPt(Double ptSize);
		void SetFontName(const UTF8Char *name);
		void SetYRefVal(Int32 refVal, Int32 col);
		void SetYRefVal(Double refVal, Int32 col);
		void SetYRefVal(Data::DateTime *refVal, Int32 col);
		void SetYRefType(Data::LineChart::RefType refType);
		void SetYUnit(const UTF8Char *yUnit);
		void SetLineThick(Double lineThick);
		void SetTimeZoneQHR(Int32 timeZone);
		void SetBarLength(Double barLength);
		void SetPointType(PointType pointType, Double pointSize);
		Int32 GetRndColor();
		void AddYDataDate(const UTF8Char *name, Int64 *value, UOSInt valCnt, Int32 lineColor, Data::LineChart::LineStyle style);
		void AddYData(const UTF8Char *name, Int32 *value, UOSInt valCnt, Int32 lineColor, Data::LineChart::LineStyle style);
		void AddYData(const UTF8Char *name, Double *value, UOSInt valCnt, Int32 lineColor, Data::LineChart::LineStyle style);
		void SetYAxis(const UTF8Char *name);
		void SetXRangeDate(Data::DateTime *xVal);
		void SetYRangeInt(Int32 yVal);
		void SetYRangeDbl(Double yVal);
//		void SetStyle(Data::LineChart::LineStyle style);
		virtual void Plot(Media::DrawImage *img, Double x, Double y, Double width, Double height);
		virtual OSInt GetLegendCount();
		virtual UTF8Char *GetLegend(UTF8Char *sbuff, Int32 *color, UOSInt index);

	};
}
#endif
