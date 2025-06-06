#include "Stdafx.h"
#include "Core/Core.h"
#include "Map/HKRoadNetwork2.h"
#include "Map/HKSpeedLimit.h"
#include "Math/CoordinateSystemManager.h"

#include <stdio.h>

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	Text::CStringNN path = CSTR("/home/sswroom/Progs/Temp/kmlTest/RdNet_IRNP.gdb");
	Math::ArcGISPRJParser prjParser;
	Map::HKRoadNetwork2 roadNetwork(path, &prjParser);
	NN<Map::HKSpeedLimit> spdLimit;
	if (roadNetwork.CreateSpeedLimit().SetTo(spdLimit))
	{
		NN<Math::CoordinateSystem> csys = Math::CoordinateSystemManager::CreateWGS84Csys();
		spdLimit->SetReqCoordinateSystem(csys);
		csys.Delete();
		Int32 speedLimit = spdLimit->GetSpeedLimit(Math::Coord2DDbl(114.230057, 22.308962), 20);
		printf("Speed Limit = %d\r\n", speedLimit);
		spdLimit.Delete();
	}
	else
	{
		printf("Error in loading speed limit data\r\n");
	}
	return 0;
}
