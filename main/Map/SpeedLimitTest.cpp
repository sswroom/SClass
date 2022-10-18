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
	spdLimit.SetReqCoordinateSystem(csys);
	DEL_CLASS(csys);
	Int32 speedLimit = spdLimit.GetSpeedLimit(Math::Coord2DDbl(114.230057, 22.308962), 20);
	printf("Speed Limit = %d\r\n", speedLimit);
	return 0;
}
