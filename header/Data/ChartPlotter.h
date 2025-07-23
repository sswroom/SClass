#ifndef _SM_DATA_CHARTPLOTTER
#define _SM_DATA_CHARTPLOTTER
#include "Media/DrawEngine.h"
#include "Data/ArrayList.h"
#include "Data/ArrayListDbl.h"
#include "Data/ArrayListStringNN.h"
#include "Data/DateTime.h"
#include "Data/RandomOS.h"
#include "Text/String.h"

namespace Data
{
	class ChartPlotter
	{
	public:
		enum class PointType
		{
			Null = 0,
			Circle = 1
		};

		enum class LineStyle
		{
			Line = 0,
			Fill = 1
		};

		enum class RefType
		{
			None = 0,
			LeftAlign = 1,
			RightAlign = 2
		};

		enum class DataType
		{
			None,
			DateTicks,
			DOUBLE,
			Integer
		};

		enum class ChartType
		{
			Line
		};
	private:
		class ChartData
		{
		public:
			NN<Text::String> name;
			void *data;
			DataType dataType;
			UInt32 lineColor;
			UOSInt dataCnt;
			LineStyle lineStyle;
			ChartType chartType;

			ChartData(NN<Text::String> name, void *data, UOSInt dataCnt, DataType dataType, UInt32 lineColor, LineStyle lineStyle, ChartType chartType);
			ChartData(Text::CStringNN name, void *data, UOSInt dataCnt, DataType dataType, UInt32 lineColor, LineStyle lineStyle, ChartType chartType);
			~ChartData();
		};
	private:
		Optional<Text::String> title;
		Optional<Text::String> xAxisName;
		Optional<Text::String> yAxisName;

		NN<Text::String> dateFormat;
		NN<Text::String> timeFormat;
		NN<Text::String> dblFormat;
		Double minDblVal;

		Data::RandomOS rnd;
		Data::ArrayList<void*> xDatas;
		Data::ArrayList<UOSInt> xDataCnt;
		DataType xType;

		Data::ArrayListNN<ChartData> yCharts;
		Optional<Text::String> titleBuff;
		Text::PString titleLine[3];
		UOSInt titleLineCnt;
		Optional<Text::String> yUnit;
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

		NN<Text::String> fntName;
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
		ChartPlotter(Text::CString title);
		~ChartPlotter();

		Bool AddXData(UnsafeArray<Data::DateTime*> data, UOSInt dataCnt);
		Bool AddXData(UnsafeArray<Data::Timestamp> data, UOSInt dataCnt);
		Bool AddXData(UnsafeArray<Double> data, UOSInt dataCnt);
		Bool AddXData(UnsafeArray<Int32> data, UOSInt dataCnt);
		Bool AddXDataDate(UnsafeArray<Int64> data, UOSInt dataCnt);
		void SetFontHeightPt(Double ptSize);
		void SetFontName(Text::CStringNN name);
		void SetYRefVal(Int32 refVal, UInt32 col);
		void SetYRefVal(Double refVal, UInt32 col);
		void SetYRefVal(NN<Data::DateTime> refVal, UInt32 col);
		void SetYRefType(RefType refType);
		void SetYUnit(Text::CString yUnit);
		void SetLineThick(Double lineThick);
		void SetTimeZoneQHR(Int8 timeZone);
		void SetBarLength(Double barLength);
		void SetPointType(PointType pointType, Double pointSize);
		UInt32 GetRndColor();
		void AddYDataDate(NN<Text::String> name, UnsafeArray<Int64> value, UOSInt valCnt, UInt32 lineColor, LineStyle style);
		void AddYDataDate(Text::CStringNN name, UnsafeArray<Int64> value, UOSInt valCnt, UInt32 lineColor, LineStyle style);
		void AddYData(NN<Text::String> name, UnsafeArray<Int32> value, UOSInt valCnt, UInt32 lineColor, LineStyle style);
		void AddYData(Text::CStringNN name, UnsafeArray<Int32> value, UOSInt valCnt, UInt32 lineColor, LineStyle style);
		void AddYData(NN<Text::String> name, UnsafeArray<Double> value, UOSInt valCnt, UInt32 lineColor, LineStyle style);
		void AddYData(Text::CStringNN name, UnsafeArray<Double> value, UOSInt valCnt, UInt32 lineColor, LineStyle style);
		void SetXRangeDate(NN<Data::DateTime> xVal);
		void SetYRangeInt(Int32 yVal);
		void SetYRangeDbl(Double yVal);
		void SetTitle(Text::CString title);
		Optional<Text::String> GetTitle() const;
		void SetDateFormat(Text::CStringNN format);
		NN<Text::String> GetDateFormat() const;
		void SetTimeFormat(Text::CStringNN format);
		NN<Text::String> GetTimeFormat() const;
		void SetDblFormat(Text::CStringNN format);
		NN<Text::String> GetDblFormat() const;

		void SetXAxisName(Text::CString xAxisName);
		Optional<Text::String> GetXAxisName() const;
		void SetYAxisName(Text::CString yAxisName);
		Optional<Text::String> GetYAxisName() const;

		DataType GetXAxisType() const;
		UOSInt GetXDataCount() const;
		UnsafeArrayOpt<Int64> GetXDateTicks(UOSInt index, OutParam<UOSInt> cnt) const;
		UnsafeArrayOpt<Double> GetXDouble(UOSInt index, OutParam<UOSInt> cnt) const;
		UnsafeArrayOpt<Int32> GetXInt32(UOSInt index, OutParam<UOSInt> cnt) const;
		UOSInt GetYDataCount() const;
		UnsafeArrayOpt<Int64> GetYDateTicks(UOSInt index, OutParam<UOSInt> cnt) const;
		UnsafeArrayOpt<Double> GetYDouble(UOSInt index, OutParam<UOSInt> cnt) const;
		UnsafeArrayOpt<Int32> GetYInt32(UOSInt index, OutParam<UOSInt> cnt) const;
		Optional<Text::String> GetYName(UOSInt index) const;
		DataType GetYType(UOSInt index) const;

		void Plot(NN<Media::DrawImage> img, Double x, Double y, Double width, Double height);
		UOSInt GetLegendCount() const;
		UnsafeArrayOpt<UTF8Char> GetLegend(UnsafeArray<UTF8Char> sbuff, OutParam<UInt32> color, UOSInt index) const;

		static UOSInt CalScaleMarkDbl(NN<Data::ArrayListDbl> locations, NN<Data::ArrayListStringNN> labels, Double min, Double max, Double leng, Double minLeng, UnsafeArray<const Char> dblFormat, Double minDblVal, Optional<Text::String> unit);
		static UOSInt CalScaleMarkInt(NN<Data::ArrayListDbl> locations, NN<Data::ArrayListStringNN> labels, Int32 min, Int32 max, Double leng, Double minLeng, Optional<Text::String> unit);
		static UOSInt CalScaleMarkDate(NN<Data::ArrayListDbl> locations, NN<Data::ArrayListStringNN> labels, NN<Data::DateTime> min, NN<Data::DateTime> max, Double leng, Double minLeng, UnsafeArray<const Char> dateFormat, UnsafeArrayOpt<const Char> timeFormat);
	};
}
#endif
