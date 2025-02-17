#ifndef _SM_SSWR_SHPCONV_RANGEFILTER
#define _SM_SSWR_SHPCONV_RANGEFILTER
#include "SSWR/SHPConv/MapFilter.h"

namespace SSWR
{
	namespace SHPConv
	{
		class RangeFilter : public MapFilter
		{
		private:
			Double left;
			Double top;
			Double right;
			Double bottom;
			Int32 compareType;

		public:
			RangeFilter(Double left, Double top, Double right, Double bottom, Int32 compareType);
			virtual ~RangeFilter();

			virtual Bool IsValid(Double left, Double top, Double right, Double bottom, NN<DB::DBReader> dbf) const;
			virtual UnsafeArray<UTF8Char> ToString(UnsafeArray<UTF8Char> buff) const;
			virtual NN<MapFilter> Clone() const;
		};
	}
}
#endif
