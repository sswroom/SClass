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
			OSInt colIndex;
			const UTF8Char *value;
			Int32 compareType;

		public:
			ValueFilter(OSInt colIndex, const UTF8Char *val, Int32 compareType);
			virtual ~ValueFilter();

			virtual Bool IsValid(Double left, Double top, Double right, Double bottom, DB::DBReader *dbf);
			virtual UTF8Char *ToString(UTF8Char *buff);
			virtual MapFilter *Clone();
		};
	};
};
#endif
