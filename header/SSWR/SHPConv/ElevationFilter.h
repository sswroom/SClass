#ifndef _SM_SSWR_SHPCONV_ELEVATIONFILTER
#define _SM_SSWR_SHPCONV_ELEVATIONFILTER
#include "SSWR/SHPConv/MapFilter.h"

namespace SSWR
{
	namespace SHPConv
	{
		class ElevationFilter : public MapFilter
		{
		private:
			UOSInt colIndex;
			Int32 value;

		public:
			ElevationFilter(UOSInt colIndex, Int32 val);
			virtual ~ElevationFilter();

			virtual Bool IsValid(Double left, Double top, Double right, Double bottom, DB::DBReader *dbf);
			virtual UTF8Char *ToString(UTF8Char *buff);
			virtual MapFilter *Clone();
		};
	}
}
#endif
