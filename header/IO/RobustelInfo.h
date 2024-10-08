#ifndef _SM_IO_ROBUSTELINFO
#define _SM_IO_ROBUSTELINFO
#include "Text/String.h"

namespace IO
{
	class RobustelInfo
	{
	public:
		static Text::String *GetIMEI();
		static Text::String *GetCellID();
		static Bool GetRSSI(Int8 *val);
		static Bool GetRSRP(Int8 *val);
		static Bool GetRSRQ(Int8 *val);
		static Bool GetPosition(Double *lat, Double *lon);
	};
}
#endif
