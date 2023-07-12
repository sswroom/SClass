#include "Stdafx.h"
#include "Core/Core.h"
#include "Map/HKRoadNetwork2.h"
#include "Map/HKSpeedLimit.h"
#include "Math/CoordinateSystemManager.h"

#include <stdio.h>

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	Text::CString path = CSTR("/home/sswroom/Progs/Temp/kmlTest/RdNet_IRNP.gdb");
	Math::ArcGISPRJParser prjParser;
	Map::HKRoadNetwork2 roadNetwork(path, &prjParser);
	Map::HKSpeedLimit *spdLimit = roadNetwork.CreateSpeedLimit();
	if (spdLimit)
	{
		Math::CoordinateSystem *csys = Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::CoordinateSystemManager::GCST_WGS84);
		spdLimit->SetReqCoordinateSystem(csys);
		DEL_CLASS(csys);
		Int32 speedLimit = spdLimit->GetSpeedLimit(Math::Coord2DDbl(114.230057, 22.308962), 20);
		printf("Speed Limit = %d\r\n", speedLimit);
		DEL_CLASS(spdLimit);
	}
	else
	{
		printf("Error in loading speed limit data\r\n");
	}
	return 0;
}
