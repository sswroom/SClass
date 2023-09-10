#ifndef _SM_MAP_TILEMAPUTIL
#define _SM_MAP_TILEMAPUTIL
#include "Math/CoordinateSystem.h"

namespace Map
{
	class TileMapUtil
	{
	public:
		static Double CalcScaleDiv(NotNullPtr<Math::CoordinateSystem> csys);
	};
}
#endif
