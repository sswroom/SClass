#ifndef _SM_IO_ROBUSTELINFO
#define _SM_IO_ROBUSTELINFO
#include "Text/String.h"

namespace IO
{
	class RobustelInfo
	{
	public:
		static Optional<Text::String> GetIMEI();
		static Optional<Text::String> GetCellID();
		static Bool GetRSSI(OutParam<Int8> val);
		static Bool GetRSRP(OutParam<Int8> val);
		static Bool GetRSRQ(OutParam<Int8> val);
		static Bool GetPosition(OutParam<Double> lat, OutParam<Double> lon);
	};
}
#endif
