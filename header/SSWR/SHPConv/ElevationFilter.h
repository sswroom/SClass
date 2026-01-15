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
			UIntOS colIndex;
			Int32 value;

		public:
			ElevationFilter(UIntOS colIndex, Int32 val);
			virtual ~ElevationFilter();

			virtual Bool IsValid(Double left, Double top, Double right, Double bottom, NN<DB::DBReader> dbf) const;
			virtual UnsafeArray<UTF8Char> ToString(UnsafeArray<UTF8Char> buff) const;
			virtual NN<MapFilter> Clone() const;
		};
	}
}
#endif
