#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/CSys/CoordinateSystemManager.h"
#include "Math/CSys/HKGCode.h"

Math::CSys::HKGCode::HKGCode()
{
	this->csysHK80 = Math::CSys::CoordinateSystemManager::CreateProjCoordinateSystemDefNameOrDef(Math::CSys::CoordinateSystemManager::PCST_HK80);
	this->csysWGS84 = Math::CSys::CoordinateSystemManager::CreateWGS84Csys();
}

Math::CSys::HKGCode::~HKGCode()
{
	this->csysHK80.Delete();
	this->csysWGS84.Delete();
}

UnsafeArray<UTF8Char> Math::CSys::HKGCode::WGS84_Grid(UnsafeArray<UTF8Char> gridStr, Double lat, Double lon)
{
	return HK80_Grid(gridStr, Math::CSys::CoordinateSystem::Convert(this->csysWGS84, this->csysHK80, Math::Coord2DDbl(lon, lat)));
}

UnsafeArray<UTF8Char> Math::CSys::HKGCode::HK80_Grid(UnsafeArray<UTF8Char> gridStr, Math::Coord2DDbl pos)
{
	// Implementation for HK80_Grid
}

Math::RectAreaDbl Math::CSys::HKGCode::Grid_HK80(Text::CStringNN gridStr)
{
	// Implementation for Grid_HK80
	return Math::RectAreaDbl(0, 0, 0, 0);
}
