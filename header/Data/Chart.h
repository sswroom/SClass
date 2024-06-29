#ifndef _SM_DATA_CHART
#define _SM_DATA_CHART
#include "Media/DrawEngine.h"
#include "Data/ArrayList.h"
#include "Data/ArrayListDbl.h"
#include "Data/ArrayListStringNN.h"
#include "Data/DateTime.h"
#include "Text/String.h"

namespace Data
{
	class Chart
	{
	public:
		enum class DataType
		{
			None,
			DateTicks,
			DOUBLE,
			Integer
		};

	protected:
		Optional<Text::String> title;
		Optional<Text::String> xAxisName;
		Optional<Text::String> yAxisName;

		NN<Text::String> dateFormat;
		NN<Text::String> timeFormat;
		NN<Text::String> dblFormat;
		Double minDblVal;

	public:
		Chart();
		virtual ~Chart();

		virtual void SetTitle(Text::CString title);
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

		virtual DataType GetXAxisType() const = 0;
		virtual UOSInt GetXDataCount() const = 0;
		virtual UnsafeArrayOpt<Int64> GetXDateTicks(UOSInt index, OutParam<UOSInt> cnt) const = 0;
		virtual UnsafeArrayOpt<Double> GetXDouble(UOSInt index, OutParam<UOSInt> cnt) const = 0;
		virtual UnsafeArrayOpt<Int32> GetXInt32(UOSInt index, OutParam<UOSInt> cnt) const = 0;
		virtual UOSInt GetYDataCount() const = 0;
		virtual UnsafeArrayOpt<Int64> GetYDateTicks(UOSInt index, OutParam<UOSInt> cnt) const = 0;
		virtual UnsafeArrayOpt<Double> GetYDouble(UOSInt index, OutParam<UOSInt> cnt) const = 0;
		virtual UnsafeArrayOpt<Int32> GetYInt32(UOSInt index, OutParam<UOSInt> cnt) const = 0;
		virtual Optional<Text::String> GetYName(UOSInt index) const = 0;
		virtual DataType GetYType(UOSInt index) const = 0;

		virtual void Plot(NN<Media::DrawImage> img, Double x, Double y, Double width, Double height) const = 0;
		virtual UOSInt GetLegendCount() const = 0;
		virtual UnsafeArrayOpt<UTF8Char> GetLegend(UnsafeArray<UTF8Char> sbuff, OutParam<UInt32> color, UOSInt index) const = 0;

		static UOSInt CalScaleMarkDbl(NN<Data::ArrayListDbl> locations, NN<Data::ArrayListStringNN> labels, Double min, Double max, Double leng, Double minLeng, UnsafeArray<const Char> dblFormat, Double minDblVal, Optional<Text::String> unit);
		static UOSInt CalScaleMarkInt(NN<Data::ArrayListDbl> locations, NN<Data::ArrayListStringNN> labels, Int32 min, Int32 max, Double leng, Double minLeng, Optional<Text::String> unit);
		static UOSInt CalScaleMarkDate(NN<Data::ArrayListDbl> locations, NN<Data::ArrayListStringNN> labels, NN<Data::DateTime> min, NN<Data::DateTime> max, Double leng, Double minLeng, UnsafeArray<const Char> dateFormat, UnsafeArrayOpt<const Char> timeFormat);
	};
}
#endif
