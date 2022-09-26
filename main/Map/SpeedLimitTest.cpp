#include "Stdafx.h"
#include "Core/Core.h"
#include "Map/HKSpeedLimit.h"
#include "Math/CoordinateSystemManager.h"

#include <stdio.h>

Int32 MyMain(Core::IProgControl *progCtrl)
{
	Text::CString path = CSTR("/home/sswroom/Progs/Temp/kmlTest/RdNet_IRNP.gdb");
	Map::HKSpeedLimit spdLimit(path);
	Math::CoordinateSystem *csys = Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::CoordinateSystemManager::GCST_WGS84);
	spdLimit.ConvertCoordinateSystem(csys);
	DEL_CLASS(csys);
	const Map::HKSpeedLimit::RouteInfo *route = spdLimit.GetNearestRoute(Math::Coord2DDbl(114.230057, 22.308962));
	if (route)
	{
		printf("Route found, speed = %d\r\n", route->speedLimit);
	}
	else
	{
		printf("Route not found\r\n");
	}
	return 0;
}
