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
			Text::String *name;
			void *data;
			Data::IChart::DataType dataType;
			UInt32 lineColor;
			UOSInt dataCnt;
			Data::LineChart::LineStyle lineStyle;

			ChartData(Text::String *name, void *data, UOSInt dataCnt, Data::IChart::DataType dataType, UInt32 lineColor, Data::LineChart::LineStyle lineStyle);
			ChartData(const UTF8Char *name, void *data, UOSInt dataCnt, Data::IChart::DataType dataType, UInt32 lineColor, Data::LineChart::LineStyle lineStyle);
			~ChartData();
		};

	private:
		Data::RandomOS *rnd;
		Data::ArrayList<void*> *xDatas;
		Data::ArrayList<UOSInt> *xDataCnt;
		DataType xType;

		Data::ArrayList<ChartData*> *yCharts;
//		Data::ArrayList *dataPos;
		const UTF8Char *titleBuff;
		UTF8Char *titleLine[3];
		UOSInt titleLineCnt;
		const UTF8Char *yUnit;
		Int8 timeZoneQHR;

		Double refDbl;
		Int32 refInt;
		Int64 refTime;
		Bool refExist;
		RefType refType;
//		LineStyle style;
		PointType pointType;
		Double pointSize;

		UInt32 bgColor;
		UInt32 boundColor;
		UInt32 fontColor;
		UInt32 gridColor;
		UInt32 refLineColor;
		Double lineThick;
		Double barLength;

		const UTF8Char *fntName;
		Double fntSizePt;

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

		Bool AddXData(Data::DateTime **data, UOSInt dataCnt);
		Bool AddXData(Double *data, UOSInt dataCnt);
		Bool AddXData(Int32 *data, UOSInt dataCnt);
		Bool AddXDataDate(Int64 *data, UOSInt dataCnt);
		void SetFontHeightPt(Double ptSize);
		void SetFontName(const UTF8Char *name);
		void SetYRefVal(Int32 refVal, UInt32 col);
		void SetYRefVal(Double refVal, UInt32 col);
		void SetYRefVal(Data::DateTime *refVal, UInt32 col);
		void SetYRefType(Data::LineChart::RefType refType);
		void SetYUnit(const UTF8Char *yUnit);
		void SetLineThick(Double lineThick);
		void SetTimeZoneQHR(Int8 timeZone);
		void SetBarLength(Double barLength);
		void SetPointType(PointType pointType, Double pointSize);
		UInt32 GetRndColor();
		void AddYDataDate(Text::String *name, Int64 *value, UOSInt valCnt, UInt32 lineColor, Data::LineChart::LineStyle style);
		void AddYDataDate(const UTF8Char *name, Int64 *value, UOSInt valCnt, UInt32 lineColor, Data::LineChart::LineStyle style);
		void AddYData(Text::String *name, Int32 *value, UOSInt valCnt, UInt32 lineColor, Data::LineChart::LineStyle style);
		void AddYData(const UTF8Char *name, Int32 *value, UOSInt valCnt, UInt32 lineColor, Data::LineChart::LineStyle style);
		void AddYData(Text::String *name, Double *value, UOSInt valCnt, UInt32 lineColor, Data::LineChart::LineStyle style);
		void AddYData(const UTF8Char *name, Double *value, UOSInt valCnt, UInt32 lineColor, Data::LineChart::LineStyle style);
		void SetXRangeDate(Data::DateTime *xVal);
		void SetYRangeInt(Int32 yVal);
		void SetYRangeDbl(Double yVal);
//		void SetStyle(Data::LineChart::LineStyle style);
		virtual void SetTitle(const UTF8Char *title);
		virtual DataType GetXAxisType();
		virtual UOSInt GetXDataCount();
		virtual Int64 *GetXDateTicks(UOSInt index, UOSInt *cnt);
		virtual Double *GetXDouble(UOSInt index, UOSInt *cnt);
		virtual Int32 *GetXInt32(UOSInt index, UOSInt *cnt);
		virtual UOSInt GetYDataCount();
		virtual Int64 *GetYDateTicks(UOSInt index, UOSInt *cnt);
		virtual Double *GetYDouble(UOSInt index, UOSInt *cnt);
		virtual Int32 *GetYInt32(UOSInt index, UOSInt *cnt);
		virtual Text::String *GetYName(UOSInt index);
		virtual DataType GetYType(UOSInt index);

		virtual void Plot(Media::DrawImage *img, Double x, Double y, Double width, Double height);
		virtual UOSInt GetLegendCount();
		virtual UTF8Char *GetLegend(UTF8Char *sbuff, UInt32 *color, UOSInt index);

	};
}
#endif
