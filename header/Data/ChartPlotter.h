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

		enum class RefType
		{
			None = 0,
			LeftAlign = 1,
			RightAlign = 2
		};

		enum class DataType
		{
			None,
			Time,
			DOUBLE,
			Integer
		};

		enum class ChartType
		{
			Line,
			FilledLine
		};

		class ChartData
		{
		protected:
			UOSInt dataCnt;

		public:
			ChartData(UOSInt dataCnt) { this->dataCnt = dataCnt; }
			virtual ~ChartData(){};

			virtual DataType GetType() const = 0;
			virtual NN<ChartData> Clone() const = 0;
			UOSInt GetCount() const { return this->dataCnt; }
		};

		class TimeData : public ChartData
		{
		private:
			UnsafeArray<Data::TimeInstant> timeArr;
		public:
			TimeData(UnsafeArray<Data::Timestamp> timeArr, UOSInt dataCnt);
			TimeData(UnsafeArray<Data::TimeInstant> timeArr, UOSInt dataCnt);
			TimeData(UnsafeArray<Int64> ticksArr, UOSInt dataCnt);
			TimeData(NN<ReadingList<Timestamp>> timeArr);
			virtual ~TimeData();

			virtual DataType GetType() const;
			virtual NN<ChartData> Clone() const;
			UnsafeArray<Data::TimeInstant> GetData() const;
		};

		class Int32Data : public ChartData
		{
		private:
			UnsafeArray<Int32> intArr;
		public:
			Int32Data(UnsafeArray<Int32> intArr, UOSInt dataCnt);
			Int32Data(NN<ReadingList<Int32>> intArr);
			virtual ~Int32Data();

			virtual DataType GetType() const;
			virtual NN<ChartData> Clone() const;
			UnsafeArray<Int32> GetData() const;
		};

		class DoubleData : public ChartData
		{
		private:
			UnsafeArray<Double> dblArr;
		public:
			DoubleData(UnsafeArray<Double> dblArr, UOSInt dataCnt);
			DoubleData(NN<ReadingList<Double>> dblArr);
			virtual ~DoubleData();

			virtual DataType GetType() const;
			virtual NN<ChartData> Clone() const;
			UnsafeArray<Double> GetData() const;
		};

		class Axis
		{
		private:
			Optional<Text::String> name;
		public:
			virtual ~Axis() { OPTSTR_DEL(this->name); };

			virtual DataType GetType() const = 0;
			virtual void CalcX(NN<ChartData> data, UnsafeArray<Math::Coord2DDbl> pos, Double minX, Double maxX) const = 0;
			virtual void CalcY(NN<ChartData> data, UnsafeArray<Math::Coord2DDbl> pos, Double minY, Double maxY) const = 0;
			void SetName(Text::CString name) { OPTSTR_DEL(this->name); this->name = Text::String::NewOrNull(name); }
			Optional<Text::String> GetName() const { return this->name; }
		};

		class TimeAxis : public Axis
		{
		private:
			Data::TimeInstant min;
			Data::TimeInstant max;
		public:
			TimeAxis(NN<TimeData> data);
			TimeAxis(Data::TimeInstant val);
			virtual ~TimeAxis();

			virtual DataType GetType() const;
			virtual void CalcX(NN<ChartData> data, UnsafeArray<Math::Coord2DDbl> pos, Double minX, Double maxX) const;
			virtual void CalcY(NN<ChartData> data, UnsafeArray<Math::Coord2DDbl> pos, Double minY, Double maxY) const;
			void ExtendRange(NN<TimeData> data);
			void ExtendRange(Data::TimeInstant inst);
			Data::TimeInstant GetMax() const { return this->max; }
			Data::TimeInstant GetMin() const { return this->min; }
		};

		class Int32Axis : public Axis
		{
		private:
			Int32 min;
			Int32 max;
		public:
			Int32Axis(NN<Int32Data> data);
			virtual ~Int32Axis();

			virtual DataType GetType() const;
			virtual void CalcX(NN<ChartData> data, UnsafeArray<Math::Coord2DDbl> pos, Double minX, Double maxX) const;
			virtual void CalcY(NN<ChartData> data, UnsafeArray<Math::Coord2DDbl> pos, Double minY, Double maxY) const;
			void ExtendRange(NN<Int32Data> data);
			void ExtendRange(Int32 v);
			Int32 GetMax() const { return this->max; }
			Int32 GetMin() const { return this->min; }
		};

		class DoubleAxis : public Axis
		{
		private:
			Double min;
			Double max;
		public:
			DoubleAxis(NN<DoubleData> data);
			virtual ~DoubleAxis();

			virtual DataType GetType() const;
			virtual void CalcX(NN<ChartData> data, UnsafeArray<Math::Coord2DDbl> pos, Double minX, Double maxX) const;
			virtual void CalcY(NN<ChartData> data, UnsafeArray<Math::Coord2DDbl> pos, Double minY, Double maxY) const;
			void ExtendRange(NN<DoubleData> data);
			void ExtendRange(Double v);
			Double GetMax() const { return this->max; }
			Double GetMin() const { return this->min; }
		};

		class ChartParam
		{
		public:
			NN<Text::String> name;
			NN<ChartData> yData;
			NN<Axis> yAxis;
			NN<ChartData> xData;
			UInt32 lineColor;
			UInt32 fillColor;
			ChartType chartType;

			ChartParam(NN<Text::String> name, NN<ChartData> yData, NN<Axis> yAxis, NN<ChartData> xData, UInt32 lineColor, UInt32 fillColor, ChartType chartType);
			ChartParam(Text::CStringNN name, NN<ChartData> yData, NN<Axis> yAxis, NN<ChartData> xData, UInt32 lineColor, UInt32 fillColor, ChartType chartType);
			~ChartParam();
		};
	private:
		Optional<Text::String> title;

		NN<Text::String> dateFormat;
		NN<Text::String> timeFormat;
		NN<Text::String> dblFormat;
		Double minDblVal;

		Data::RandomOS rnd;

		Data::ArrayListNN<ChartParam> charts;
		Optional<Text::String> titleBuff;
		Text::PString titleLine[3];
		UOSInt titleLineCnt;
		Optional<Text::String> yUnit;
		Int8 timeZoneQHR;

		Double refDbl;
		Int32 refInt;
		Data::TimeInstant refTime;
		Bool refExist;
		RefType refType;
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

		Optional<Axis> xAxis;
		Optional<Axis> y1Axis;
		Optional<Axis> y2Axis;

	private:
		Optional<Axis> GetXAxis(NN<ChartData> data);
		Optional<Axis> GetYAxis(NN<ChartData> data);
	public:
		ChartPlotter(Text::CString title);
		~ChartPlotter();

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
		Bool AddLineChart(NN<Text::String> name, NN<ChartData> yData, NN<ChartData> xData, UInt32 lineColor);
		Bool AddLineChart(Text::CStringNN name, NN<ChartData> yData, NN<ChartData> xData, UInt32 lineColor);
		Bool AddFilledLineChart(NN<Text::String> name, NN<ChartData> yData, NN<ChartData> xData, UInt32 lineColor, UInt32 fillColor);
		Bool AddFilledLineChart(Text::CStringNN name, NN<ChartData> yData, NN<ChartData> xData, UInt32 lineColor, UInt32 fillColor);
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
		void SetY1AxisName(Text::CString y1AxisName);
		Optional<Text::String> GetY1AxisName() const;
		void SetY2AxisName(Text::CString y2AxisName);
		Optional<Text::String> GetY2AxisName() const;

		DataType GetXAxisType() const;
		UOSInt GetChartCount() const;
		Optional<ChartParam> GetChart(UOSInt index) const;

		void Plot(NN<Media::DrawImage> img, Double x, Double y, Double width, Double height);
		UOSInt GetLegendCount() const;
		UnsafeArrayOpt<UTF8Char> GetLegend(UnsafeArray<UTF8Char> sbuff, OutParam<UInt32> color, UOSInt index) const;

		static UOSInt CalScaleMarkDbl(NN<Data::ArrayListDbl> locations, NN<Data::ArrayListStringNN> labels, Double min, Double max, Double leng, Double minLeng, UnsafeArray<const Char> dblFormat, Double minDblVal, Optional<Text::String> unit);
		static UOSInt CalScaleMarkInt(NN<Data::ArrayListDbl> locations, NN<Data::ArrayListStringNN> labels, Int32 min, Int32 max, Double leng, Double minLeng, Optional<Text::String> unit);
		static UOSInt CalScaleMarkDate(NN<Data::ArrayListDbl> locations, NN<Data::ArrayListStringNN> labels, NN<Data::DateTime> min, NN<Data::DateTime> max, Double leng, Double minLeng, UnsafeArray<const Char> dateFormat, UnsafeArrayOpt<const Char> timeFormat);

		static NN<TimeData> NewData(UnsafeArray<Data::Timestamp> data, UOSInt dataCnt);
		static NN<Int32Data> NewData(UnsafeArray<Int32> data, UOSInt dataCnt);
		static NN<DoubleData> NewData(UnsafeArray<Double> data, UOSInt dataCnt);
		static NN<TimeData> NewDataDate(UnsafeArray<Int64> ticksData, UOSInt dataCnt);
		static NN<TimeData> NewData(NN<Data::ReadingList<Data::Timestamp>> data);
		static NN<Int32Data> NewData(NN<Data::ReadingList<Int32>> data);
		static NN<DoubleData> NewData(NN<Data::ReadingList<Double>> data);
		static Optional<Axis> NewAxis(NN<ChartData> data);
	};
}
#endif
