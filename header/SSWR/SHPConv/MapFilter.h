#ifndef _SM_SSWR_SHPCONV_MAPFILTER
#define _SM_SSWR_SHPCONV_MAPFILTER
#include "DB/DBReader.h"

namespace SSWR
{
	namespace SHPConv
	{
		class MapFilter
		{
		public:
			virtual ~MapFilter() {};

			virtual Bool IsValid(Double left, Double top, Double right, Double bottom, NN<DB::DBReader> dbf) const = 0;
			virtual UnsafeArray<UTF8Char> ToString(UnsafeArray<UTF8Char> buff) const = 0;
			virtual NN<MapFilter> Clone() const = 0;
		};
	}
}
#endif
