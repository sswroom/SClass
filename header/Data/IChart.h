#ifndef _SM_DATA_ICHART
#define _SM_DATA_ICHART
#include "Media/DrawEngine.h"
#include "Data/ArrayList.h"
#include "Data/ArrayListDbl.h"
#include "Data/DateTime.h"

namespace Data
{
	class IChart
	{
	public:
		virtual ~IChart(){};

		virtual void Plot(Media::DrawImage *img, Double x, Double y, Double width, Double height) = 0;
		virtual UOSInt GetLegendCount() = 0;
		virtual UTF8Char *GetLegend(UTF8Char *sbuff, UInt32 *color, UOSInt index) = 0;

		static UOSInt CalScaleMarkDbl(Data::ArrayListDbl *locations, Data::ArrayList<const UTF8Char*> *labels, Double min, Double max, Double leng, Double minLeng, const Char *dblFormat, Double minDblVal, const UTF8Char *unit);
		static UOSInt CalScaleMarkInt(Data::ArrayListDbl *locations, Data::ArrayList<const UTF8Char*> *labels, Int32 min, Int32 max, Double leng, Double minLeng, const UTF8Char *unit);
		static UOSInt CalScaleMarkDate(Data::ArrayListDbl *locations, Data::ArrayList<const UTF8Char*> *labels, Data::DateTime *min, Data::DateTime *max, Double leng, Double minLeng, const Char *dateFormat, const Char *timeFormat);
	};
}
#endif
