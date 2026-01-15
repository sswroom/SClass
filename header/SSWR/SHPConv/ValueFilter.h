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
			UIntOS colIndex;
			NN<Text::String> value;
			Int32 compareType;

		public:
			ValueFilter(UIntOS colIndex, Text::CStringNN val, Int32 compareType);
			virtual ~ValueFilter();

			virtual Bool IsValid(Double left, Double top, Double right, Double bottom, NN<DB::DBReader> dbf) const;
			virtual UnsafeArray<UTF8Char> ToString(UnsafeArray<UTF8Char> buff) const;
			virtual NN<MapFilter> Clone() const;
		};
	}
}
#endif
