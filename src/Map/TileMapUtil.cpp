#include "Stdafx.h"
#include "Map/TileMapUtil.h"
#include "Math/Unit/Distance.h"

Double Map::TileMapUtil::CalcScaleDiv(NotNullPtr<Math::CoordinateSystem> csys)
{
	if (csys->IsProjected())
	{
		return Math::Unit::Distance::Convert(Math::Unit::Distance::DU_PIXEL, Math::Unit::Distance::DU_METER, 1);
	}
	else
	{
		return Math::Unit::Distance::Convert(Math::Unit::Distance::DU_PIXEL, Math::Unit::Distance::DU_METER, 0.000005);
	}
}
