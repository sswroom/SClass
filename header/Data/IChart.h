#ifndef _SM_DATA_ICHART
#define _SM_DATA_ICHART
#include "Media/DrawEngine.h"
#include "Data/ArrayList.h"
#include "Data/ArrayListDbl.h"
#include "Data/DateTime.h"
#include "Text/String.h"

namespace Data
{
	class IChart
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
		const UTF8Char *xAxisName;
		const UTF8Char *yAxisName;

		Text::String *dateFormat;
		Text::String *timeFormat;
		Text::String *dblFormat;
		Double minDblVal;

	public:
		IChart();
		virtual ~IChart();

		virtual void SetTitle(Text::CString title);
		Text::String *GetTitle();
		void SetDateFormat(Text::CString format);
		Text::String *GetDateFormat();
		void SetTimeFormat(Text::CString format);
		Text::String *GetTimeFormat();
		void SetDblFormat(Text::CString format);
		Text::String *GetDblFormat();

		void SetXAxisName(const UTF8Char *xAxisName);
		const UTF8Char *GetXAxisName();
		void SetYAxisName(const UTF8Char *yAxisName);
		const UTF8Char *GetYAxisName();

		virtual DataType GetXAxisType() = 0;
		virtual UOSInt GetXDataCount() = 0;
		virtual Int64 *GetXDateTicks(UOSInt index, UOSInt *cnt) = 0;
		virtual Double *GetXDouble(UOSInt index, UOSInt *cnt) = 0;
		virtual Int32 *GetXInt32(UOSInt index, UOSInt *cnt) = 0;
		virtual UOSInt GetYDataCount() = 0;
		virtual Int64 *GetYDateTicks(UOSInt index, UOSInt *cnt) = 0;
		virtual Double *GetYDouble(UOSInt index, UOSInt *cnt) = 0;
		virtual Int32 *GetYInt32(UOSInt index, UOSInt *cnt) = 0;
		virtual Text::String *GetYName(UOSInt index) = 0;
		virtual DataType GetYType(UOSInt index) = 0;

		virtual void Plot(Media::DrawImage *img, Double x, Double y, Double width, Double height) = 0;
		virtual UOSInt GetLegendCount() = 0;
		virtual UTF8Char *GetLegend(UTF8Char *sbuff, UInt32 *color, UOSInt index) = 0;

		static UOSInt CalScaleMarkDbl(Data::ArrayListDbl *locations, Data::ArrayList<Text::String*> *labels, Double min, Double max, Double leng, Double minLeng, const Char *dblFormat, Double minDblVal, const UTF8Char *unit);
		static UOSInt CalScaleMarkInt(Data::ArrayListDbl *locations, Data::ArrayList<Text::String*> *labels, Int32 min, Int32 max, Double leng, Double minLeng, const UTF8Char *unit);
		static UOSInt CalScaleMarkDate(Data::ArrayListDbl *locations, Data::ArrayList<Text::String*> *labels, Data::DateTime *min, Data::DateTime *max, Double leng, Double minLeng, const Char *dateFormat, const Char *timeFormat);
	};
}
#endif
