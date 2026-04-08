#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Math/CoordinateSystemManager.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	NN<Math::GeographicCoordinateSystem> coord = Math::CoordinateSystemManager::CreateWGS84Csys();
	Double polarAxis = coord->GetEllipsoid()->GetSemiMinorAxis();
	Double dist = coord->CalSurfaceDistance(Math::Coord2DDbl(114.151558, 22.335974), Math::Coord2DDbl(114.151558, 22.335974), Math::Unit::Distance::DU_METER);
	printf("Polar Axis = %lf, Dist = %lf(m)\r\n", polarAxis, dist);
	coord.Delete();
	return 0;
}
