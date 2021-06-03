#ifndef _SM_MAP_SHPUTIL
#define _SM_MAP_SHPUTIL
#include "Math/Vector2D.h"

namespace Map
{
	class SHPUtil
	{
	public:
		static Math::Vector2D *ParseShpRecord(UInt32 srid, const UInt8 *buff, UOSInt buffSize);
	};
}
#endif
