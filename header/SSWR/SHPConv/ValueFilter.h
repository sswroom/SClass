#ifndef _SM_SSWR_SHPCONV_VALUEFILTER
#define _SM_SSWR_SHPCONV_VALUEFILTER
#include "SSWR/SHPConv/MapFilter.h"

namespace SSWR
{
	namespace SHPConv
	{
		class ValueFilter : public MapFilter
		{
		private:
			UOSInt colIndex;
			NN<Text::String> value;
			Int32 compareType;

		public:
			ValueFilter(UOSInt colIndex, Text::CString val, Int32 compareType);
			virtual ~ValueFilter();

			virtual Bool IsValid(Double left, Double top, Double right, Double bottom, NN<DB::DBReader> dbf) const;
			virtual UTF8Char *ToString(UTF8Char *buff) const;
			virtual MapFilter *Clone() const;
		};
	}
}
#endif
