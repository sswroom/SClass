#ifndef _SM_MAP_TILEMAPUTIL
#define _SM_MAP_TILEMAPUTIL
#include "Math/CSys/CoordinateSystem.h"

namespace Map
{
	class TileMapUtil
	{
	public:
		static Double CalcScaleDiv(NN<Math::CSys::CoordinateSystem> csys);
	};
}
#endif
