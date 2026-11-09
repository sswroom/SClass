#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/CSys/CoordinateSystemManager.h"
#include "Math/CSys/HKGCode.h"

UnsafeArray<const UTF8Char> Math::CSys::HKGCode::listCode = U8STR("0123456789ABCDEFGHJKMNPQRSTVWXYZ");

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
	if (pos.y < 800000 || pos.x < 800000 || pos.y >= 848000 || pos.x >= 864000)
	{
		gridStr[0] = 0;
		return gridStr;
	}
	Int32 e0 = 800000; //HK80 Origin Easting
	Int32 n0 = 800000; //HK80 Origin Northing
	Int32 eL1, nL1; // Level 1 Grid 2km x 2km
	Int32 eL2, nL2; // Level 2 Grid 100m x 100m
	Int32 eL3, nL3; // Level 3 Grid 5m x 5m
	eL1 = (Int32)((pos.x - e0) / 2000);
	nL1 = (Int32)((pos.y - n0) / 2000);
	gridStr[0] = listCode[eL1];
	gridStr[1] = listCode[nL1];
	eL2 = (Int32)(((pos.x - e0) - (eL1 * 2000)) / 100);
	nL2 = (Int32)(((pos.y - n0) - (nL1 * 2000)) / 100);
	gridStr[2] = listCode[eL2];
	gridStr[3] = listCode[nL2];
	eL3 = (Int32)(((pos.x - e0) - (eL1 * 2000) - (eL2 * 100)) / 5);
	nL3 = (Int32)(((pos.y - n0) - (nL1 * 2000) - (nL2 * 100)) / 5);
	gridStr[4] = listCode[eL3];
	gridStr[5] = listCode[nL3];
	gridStr[6] = 0;
	return gridStr + 6;
}

Math::RectAreaDbl Math::CSys::HKGCode::Grid_HK80(Text::CStringNN gridStr)
{
	Double e;
	Double n;
	UIntOS eL;
	UIntOS nL;
	if (gridStr.leng != 2 && gridStr.leng != 4 && gridStr.leng != 6)
	{
		return Math::RectAreaDbl(0, 0, 0, 0);
	}
	eL = Text::StrIndexOfChar(listCode, gridStr.v[0]);
	nL = Text::StrIndexOfChar(listCode, gridStr.v[1]);
	if (eL == INVALID_INDEX || nL == INVALID_INDEX)
	{
		return Math::RectAreaDbl(0, 0, 0, 0);
	}
	e = 800000 + UIntOS2Double(eL) * 2000;
	n = 800000 + UIntOS2Double(nL) * 2000;
	if (gridStr.leng == 2)
	{
		return Math::RectAreaDbl(e, n, e + 2000, n + 2000);
	}
	eL = Text::StrIndexOfChar(listCode, gridStr.v[2]);
	nL = Text::StrIndexOfChar(listCode, gridStr.v[3]);
	if (eL == INVALID_INDEX || nL == INVALID_INDEX)
	{
		return Math::RectAreaDbl(0, 0, 0, 0);
	}
	e += UIntOS2Double(eL) * 100;
	n += UIntOS2Double(nL) * 100;
	if (gridStr.leng == 4)
	{
		return Math::RectAreaDbl(e, n, e + 100, n + 100);
	}
	eL = Text::StrIndexOfChar(listCode, gridStr.v[4]);
	nL = Text::StrIndexOfChar(listCode, gridStr.v[5]);
	if (eL == INVALID_INDEX || nL == INVALID_INDEX)
	{
		return Math::RectAreaDbl(0, 0, 0, 0);
	}
	e += UIntOS2Double(eL) * 5;
	n += UIntOS2Double(nL) * 5;
	return Math::RectAreaDbl(e, n, e + 5, n + 5);
}
