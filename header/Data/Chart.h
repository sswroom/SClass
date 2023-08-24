#ifndef _SM_DATA_CHART
#define _SM_DATA_CHART
#include "Media/DrawEngine.h"
#include "Data/ArrayList.h"
#include "Data/ArrayListDbl.h"
#include "Data/ArrayListNN.h"
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
		Text::String *title;
		Text::String *xAxisName;
		Text::String *yAxisName;

		NotNullPtr<Text::String> dateFormat;
		NotNullPtr<Text::String> timeFormat;
		NotNullPtr<Text::String> dblFormat;
		Double minDblVal;

	public:
		Chart();
		virtual ~Chart();

		virtual void SetTitle(Text::CString title);
		Text::String *GetTitle() const;
		void SetDateFormat(Text::CString format);
		NotNullPtr<Text::String> GetDateFormat() const;
		void SetTimeFormat(Text::CString format);
		NotNullPtr<Text::String> GetTimeFormat() const;
		void SetDblFormat(Text::CString format);
		NotNullPtr<Text::String> GetDblFormat() const;

		void SetXAxisName(Text::CString xAxisName);
		Text::String *GetXAxisName() const;
		void SetYAxisName(Text::CString yAxisName);
		Text::String *GetYAxisName() const;

		virtual DataType GetXAxisType() const = 0;
		virtual UOSInt GetXDataCount() const = 0;
		virtual Int64 *GetXDateTicks(UOSInt index, UOSInt *cnt) const = 0;
		virtual Double *GetXDouble(UOSInt index, UOSInt *cnt) const = 0;
		virtual Int32 *GetXInt32(UOSInt index, UOSInt *cnt) const = 0;
		virtual UOSInt GetYDataCount() const = 0;
		virtual Int64 *GetYDateTicks(UOSInt index, UOSInt *cnt) const = 0;
		virtual Double *GetYDouble(UOSInt index, UOSInt *cnt) const = 0;
		virtual Int32 *GetYInt32(UOSInt index, UOSInt *cnt) const = 0;
		virtual Text::String *GetYName(UOSInt index) const = 0;
		virtual DataType GetYType(UOSInt index) const = 0;

		virtual void Plot(Media::DrawImage *img, Double x, Double y, Double width, Double height) const = 0;
		virtual UOSInt GetLegendCount() const = 0;
		virtual UTF8Char *GetLegend(UTF8Char *sbuff, UInt32 *color, UOSInt index) const = 0;

		static UOSInt CalScaleMarkDbl(Data::ArrayListDbl *locations, Data::ArrayListNN<Text::String> *labels, Double min, Double max, Double leng, Double minLeng, const Char *dblFormat, Double minDblVal, const UTF8Char *unit);
		static UOSInt CalScaleMarkInt(Data::ArrayListDbl *locations, Data::ArrayListNN<Text::String> *labels, Int32 min, Int32 max, Double leng, Double minLeng, const UTF8Char *unit);
		static UOSInt CalScaleMarkDate(Data::ArrayListDbl *locations, Data::ArrayListNN<Text::String> *labels, NotNullPtr<Data::DateTime> min, NotNullPtr<Data::DateTime> max, Double leng, Double minLeng, const Char *dateFormat, const Char *timeFormat);
	};
}
#endif
