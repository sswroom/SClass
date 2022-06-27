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

			virtual Bool IsValid(Double left, Double top, Double right, Double bottom, DB::DBReader *dbf) const = 0;
			virtual UTF8Char *ToString(UTF8Char *buff) const = 0;
			virtual MapFilter *Clone() const = 0;
		};
	}
}
#endif
