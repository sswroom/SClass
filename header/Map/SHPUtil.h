#ifndef _SM_MAP_SHPUTIL
#define _SM_MAP_SHPUTIL
#include "Math/Geometry/Vector2D.h"

namespace Map
{
	class SHPUtil
	{
	public:
		static Optional<Math::Geometry::Vector2D> ParseShpRecord(UInt32 srid, UnsafeArray<const UInt8> buff, UIntOS buffSize);
	};
}
#endif
