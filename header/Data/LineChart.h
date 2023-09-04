#ifndef _SM_DATA_LINECHART
#define _SM_DATA_LINECHART
#include "Data/ArrayList.h"
#include "Data/ArrayListInt32.h"
#include "Data/DateTime.h"
#include "Data/Chart.h"
#include "Data/RandomOS.h"

namespace Data
{
	
	class LineChart : public Data::Chart
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
			NotNullPtr<Text::String> name;
			void *data;
			Data::Chart::DataType dataType;
			UInt32 lineColor;
			UOSInt dataCnt;
			Data::LineChart::LineStyle lineStyle;

			ChartData(Text::String *name, void *data, UOSInt dataCnt, Data::Chart::DataType dataType, UInt32 lineColor, Data::LineChart::LineStyle lineStyle);
			ChartData(Text::CString name, void *data, UOSInt dataCnt, Data::Chart::DataType dataType, UInt32 lineColor, Data::LineChart::LineStyle lineStyle);
			~ChartData();
		};

	private:
		Data::RandomOS *rnd;
		Data::ArrayList<void*> *xDatas;
		Data::ArrayList<UOSInt> *xDataCnt;
		DataType xType;

		Data::ArrayList<ChartData*> *yCharts;
//		Data::ArrayList *dataPos;
		Text::String *titleBuff;
		Text::PString titleLine[3];
		UOSInt titleLineCnt;
		Text::String *yUnit;
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

		NotNullPtr<Text::String> fntName;
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
		LineChart(Text::CString title);
		virtual ~LineChart();

		Bool AddXData(Data::DateTime **data, UOSInt dataCnt);
		Bool AddXData(Data::Timestamp *data, UOSInt dataCnt);
		Bool AddXData(Double *data, UOSInt dataCnt);
		Bool AddXData(Int32 *data, UOSInt dataCnt);
		Bool AddXDataDate(Int64 *data, UOSInt dataCnt);
		void SetFontHeightPt(Double ptSize);
		void SetFontName(Text::CString name);
		void SetYRefVal(Int32 refVal, UInt32 col);
		void SetYRefVal(Double refVal, UInt32 col);
		void SetYRefVal(Data::DateTime *refVal, UInt32 col);
		void SetYRefType(Data::LineChart::RefType refType);
		void SetYUnit(Text::CString yUnit);
		void SetLineThick(Double lineThick);
		void SetTimeZoneQHR(Int8 timeZone);
		void SetBarLength(Double barLength);
		void SetPointType(PointType pointType, Double pointSize);
		UInt32 GetRndColor();
		void AddYDataDate(Text::String *name, Int64 *value, UOSInt valCnt, UInt32 lineColor, Data::LineChart::LineStyle style);
		void AddYDataDate(Text::CString name, Int64 *value, UOSInt valCnt, UInt32 lineColor, Data::LineChart::LineStyle style);
		void AddYData(Text::String *name, Int32 *value, UOSInt valCnt, UInt32 lineColor, Data::LineChart::LineStyle style);
		void AddYData(Text::CString name, Int32 *value, UOSInt valCnt, UInt32 lineColor, Data::LineChart::LineStyle style);
		void AddYData(Text::String *name, Double *value, UOSInt valCnt, UInt32 lineColor, Data::LineChart::LineStyle style);
		void AddYData(Text::CString name, Double *value, UOSInt valCnt, UInt32 lineColor, Data::LineChart::LineStyle style);
		void SetXRangeDate(Data::DateTime *xVal);
		void SetYRangeInt(Int32 yVal);
		void SetYRangeDbl(Double yVal);
//		void SetStyle(Data::LineChart::LineStyle style);
		virtual void SetTitle(Text::CString title);
		virtual DataType GetXAxisType() const;
		virtual UOSInt GetXDataCount() const;
		virtual Int64 *GetXDateTicks(UOSInt index, UOSInt *cnt) const;
		virtual Double *GetXDouble(UOSInt index, UOSInt *cnt) const;
		virtual Int32 *GetXInt32(UOSInt index, UOSInt *cnt) const;
		virtual UOSInt GetYDataCount() const;
		virtual Int64 *GetYDateTicks(UOSInt index, UOSInt *cnt) const;
		virtual Double *GetYDouble(UOSInt index, UOSInt *cnt) const;
		virtual Int32 *GetYInt32(UOSInt index, UOSInt *cnt) const;
		virtual Text::String *GetYName(UOSInt index) const;
		virtual DataType GetYType(UOSInt index) const;

		virtual void Plot(NotNullPtr<Media::DrawImage> img, Double x, Double y, Double width, Double height) const;
		virtual UOSInt GetLegendCount() const;
		virtual UTF8Char *GetLegend(UTF8Char *sbuff, UInt32 *color, UOSInt index) const;

	};
}
#endif
