#ifndef _SM_DATA_CHARTPLOTTER
#define _SM_DATA_CHARTPLOTTER
#include "Media/DrawEngine.h"
#include "Data/ArrayListDbl.h"
#include "Data/ArrayListNative.hpp"
#include "Data/ArrayListStringNN.h"
#include "Data/DateTime.h"
#include "Data/RandomOS.h"
#include "Data/TwinItemNative.hpp"
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
			Integer,
			UInteger
		};

		enum class ChartType
		{
			Line,
			FilledLine,
			Histogram,
			Scatter
		};

		enum class GridType
		{
			None,
			Horizontal,
			Vertical,
			Both
		};

		class ChartData
		{
		protected:
			UIntOS dataCnt;

		public:
			ChartData(UIntOS dataCnt) { this->dataCnt = dataCnt; }
			virtual ~ChartData(){};

			virtual DataType GetType() const = 0;
			virtual NN<ChartData> Clone() const = 0;
			UIntOS GetCount() const { return this->dataCnt; }
			void UpdateCount(UIntOS newCnt) { if (newCnt <= this->dataCnt) this->dataCnt = newCnt; }
		};

		template <typename T> class ArrayChartData : public ChartData
		{
		protected:
			UnsafeArray<T> dataArr;
		public:
			ArrayChartData(UIntOS dataCnt) : ChartData(dataCnt)
			{
				this->dataArr = MemAllocArr(T, dataCnt);
			}

			virtual ~ArrayChartData()
			{
				MemFreeArr(this->dataArr);
			}

			UnsafeArray<T> GetData() const
			{
				return this->dataArr;
			}
		};

		class TimeData : public ArrayChartData<Data::TimeInstant>
		{
		public:
			TimeData(UnsafeArray<Data::Timestamp> timeArr, UIntOS dataCnt);
			TimeData(UnsafeArray<Data::TimeInstant> timeArr, UIntOS dataCnt);
			TimeData(UnsafeArray<Int64> ticksArr, UIntOS dataCnt);
			TimeData(NN<ReadingList<Timestamp>> timeArr);
			virtual ~TimeData();

			virtual DataType GetType() const;
			virtual NN<ChartData> Clone() const;

			template <typename V> void KeepAfter(Data::Timestamp ts, NN<ArrayChartData<V>> refData)
			{
				UnsafeArray<V> refArr = refData->GetData();
				UnsafeArray<Data::TimeInstant> thisArr = this->dataArr;
				UIntOS newCnt = 0;
				UIntOS i = 0;
				UIntOS j = this->dataCnt;
				while (i < j)
				{
					if (thisArr[i] >= ts.inst)
					{
						thisArr[newCnt] = thisArr[i];
						refArr[newCnt] = refArr[i];
						newCnt++;
					}
					i++;
				}
				this->dataCnt = newCnt;
				refData->UpdateCount(newCnt);
			}
		};

		class Int32Data : public ArrayChartData<Int32>
		{
		public:
			Int32Data(UnsafeArray<Int32> intArr, UIntOS dataCnt);
			Int32Data(NN<ReadingList<Int32>> intArr);
			virtual ~Int32Data();

			virtual DataType GetType() const;
			virtual NN<ChartData> Clone() const;
		};

		class UInt32Data : public ArrayChartData<UInt32>
		{
		public:
			UInt32Data(UnsafeArray<UInt32> intArr, UIntOS dataCnt);
			UInt32Data(NN<ReadingList<UInt32>> intArr);
			virtual ~UInt32Data();

			virtual DataType GetType() const;
			virtual NN<ChartData> Clone() const;
		};

		class DoubleData : public ArrayChartData<Double>
		{
		public:
			DoubleData(UnsafeArray<Double> dblArr, UIntOS dataCnt);
			DoubleData(NN<ReadingList<Double>> dblArr);
			virtual ~DoubleData();

			virtual DataType GetType() const;
			virtual NN<ChartData> Clone() const;
			NN<DoubleData> Invert();
		};

		class Axis
		{
		private:
			Optional<Text::String> name;
			Double labelRotate;
		public:
			Axis() { this->name = nullptr; this->labelRotate = 0; }
			virtual ~Axis() { OPTSTR_DEL(this->name); };

			virtual DataType GetType() const = 0;
			virtual void CalcX(NN<ChartData> data, UnsafeArray<Math::Coord2DDbl> pos, Double minX, Double maxX) const = 0;
			virtual void CalcY(NN<ChartData> data, UnsafeArray<Math::Coord2DDbl> pos, Double minY, Double maxY) const = 0;
			void SetName(Text::CString name) { OPTSTR_DEL(this->name); this->name = Text::String::NewOrNull(name); }
			Optional<Text::String> GetName() const { return this->name; }
			void SetLabelRotate(Double labelRotate) { this->labelRotate = labelRotate; }
			Double GetLabelRotate() const { return this->labelRotate; }
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

		class UInt32Axis : public Axis
		{
		private:
			UInt32 min;
			UInt32 max;
		public:
			UInt32Axis(NN<UInt32Data> data);
			virtual ~UInt32Axis();

			virtual DataType GetType() const;
			virtual void CalcX(NN<ChartData> data, UnsafeArray<Math::Coord2DDbl> pos, Double minX, Double maxX) const;
			virtual void CalcY(NN<ChartData> data, UnsafeArray<Math::Coord2DDbl> pos, Double minY, Double maxY) const;
			void ExtendRange(NN<UInt32Data> data);
			void ExtendRange(UInt32 v);
			UInt32 GetMax() const { return this->max; }
			UInt32 GetMin() const { return this->min; }
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
			UnsafeArrayOpt<Optional<Text::String>> labels;

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
		UIntOS titleLineCnt;
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
		GridType gridType;

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
		void SetGridType(GridType gridType);
		UInt32 GetRndColor();
		Bool AddLineChart(NN<Text::String> name, NN<ChartData> yData, NN<ChartData> xData, UInt32 lineColor);
		Bool AddLineChart(Text::CStringNN name, NN<ChartData> yData, NN<ChartData> xData, UInt32 lineColor);
		Bool AddFilledLineChart(NN<Text::String> name, NN<ChartData> yData, NN<ChartData> xData, UInt32 lineColor, UInt32 fillColor);
		Bool AddFilledLineChart(Text::CStringNN name, NN<ChartData> yData, NN<ChartData> xData, UInt32 lineColor, UInt32 fillColor);
		Bool AddScatter(Text::CStringNN name, NN<ChartData> xdata, NN<ChartData> ydata, UInt32 lineColor);
		Bool AddScatter(Text::CStringNN name, NN<ChartData> xdata, NN<ChartData> ydata, UnsafeArrayOpt<Optional<Text::String>> labels, UInt32 lineColor);
		Bool AddHistogramCount(Text::CStringNN name, NN<ChartData> data, UIntOS barCount, UInt32 lineColor, UInt32 fillColor);

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

		Optional<Axis> GetXAxis() const;
		Optional<Axis> GetY1Axis() const;
		Optional<Axis> GetY2Axis() const;
		DataType GetXAxisType() const;
		UIntOS GetChartCount() const;
		Optional<ChartParam> GetChart(UIntOS index) const;

		void Plot(NN<Media::DrawImage> img, Double x, Double y, Double width, Double height);
		UIntOS GetLegendCount() const;
		UnsafeArrayOpt<UTF8Char> GetLegend(UnsafeArray<UTF8Char> sbuff, OutParam<UInt32> color, UIntOS index) const;
		Bool SavePng(NN<Media::DrawEngine> deng, Math::Size2D<UIntOS> size, Text::CStringNN fileName);

		static UIntOS CalScaleMarkDbl(NN<Data::ArrayListDbl> locations, NN<Data::ArrayListStringNN> labels, Double min, Double max, Double leng, Double minLeng, UnsafeArray<const Char> dblFormat, Double minDblVal, Optional<Text::String> unit);
		static UIntOS CalScaleMarkInt(NN<Data::ArrayListDbl> locations, NN<Data::ArrayListStringNN> labels, Int32 min, Int32 max, Double leng, Double minLeng, Optional<Text::String> unit);
		static UIntOS CalScaleMarkUInt(NN<Data::ArrayListDbl> locations, NN<Data::ArrayListStringNN> labels, UInt32 min, UInt32 max, Double leng, Double minLeng, Optional<Text::String> unit);
		static UIntOS CalScaleMarkDate(NN<Data::ArrayListDbl> locations, NN<Data::ArrayListStringNN> labels, NN<Data::DateTime> min, NN<Data::DateTime> max, Double leng, Double minLeng, UnsafeArray<const Char> dateFormat, UnsafeArrayOpt<const Char> timeFormat);

		static NN<TimeData> NewData(UnsafeArray<Data::Timestamp> data, UIntOS dataCnt);
		static NN<Int32Data> NewData(UnsafeArray<Int32> data, UIntOS dataCnt);
		static NN<UInt32Data> NewData(UnsafeArray<UInt32> data, UIntOS dataCnt);
		static NN<DoubleData> NewData(UnsafeArray<Double> data, UIntOS dataCnt);
		static NN<TimeData> NewDataDate(UnsafeArray<Int64> ticksData, UIntOS dataCnt);
		static NN<TimeData> NewData(NN<Data::ReadingList<Data::Timestamp>> data);
		static NN<Int32Data> NewData(NN<Data::ReadingList<Int32>> data);
		static NN<UInt32Data> NewData(NN<Data::ReadingList<UInt32>> data);
		static NN<DoubleData> NewData(NN<Data::ReadingList<Double>> data);
		static NN<Int32Data> NewDataSeq(Int32 startSeq, UIntOS count);
		static Optional<Axis> NewAxis(NN<ChartData> data);

		template<class K, class V> static NN<ChartData> NewDataFromKey(NN<Data::ArrayListNative<TwinItemNative<K, V>>> vals)
		{
			UIntOS i = 0;
			UIntOS j = vals->GetCount();
			Data::ArrayListNative<K> arr(j);
			while (i < j)
			{
				arr.Add(vals->GetItem(i).key);
				i++;
			}
			return NewData(arr);
		}

		template<class K, class V> static NN<ChartData> NewDataFromValue(NN<Data::ArrayListNative<TwinItemNative<K, V>>> vals)
		{
			UIntOS i = 0;
			UIntOS j = vals->GetCount();
			Data::ArrayListNative<V> arr(j);
			while (i < j)
			{
				arr.Add(vals->GetItem(i).value);
				i++;
			}
			return NewData(arr);
		}
	};
}
#endif
