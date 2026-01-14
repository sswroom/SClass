#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math_C.h"
#include "Math/PointMappingCoordinateSystem.h"
#include "Math/Geometry/Polyline.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

Math::PointMappingCoordinateSystem::PointMappingCoordinateSystem(NN<Text::String> sourceName, UInt32 srid, Text::CStringNN csysName, NN<Math::CoordinateSystem> baseCSys) : Math::CoordinateSystem(sourceName, srid, csysName)
{
	this->baseCSys = baseCSys;
}

Math::PointMappingCoordinateSystem::PointMappingCoordinateSystem(Text::CStringNN sourceName, UInt32 srid, Text::CStringNN csysName, NN<Math::CoordinateSystem> baseCSys) : Math::CoordinateSystem(sourceName, srid, csysName)
{
	this->baseCSys = baseCSys;
}

Math::PointMappingCoordinateSystem::~PointMappingCoordinateSystem()
{
	UOSInt i;
	UnsafeArray<Double> ptItem;
	i = this->mappingList.GetCount();
	while (i-- > 0)
	{
		ptItem = this->mappingList.GetItemNoCheck(i);
		MemFreeArr(ptItem);
	}
	this->baseCSys.Delete();
}

void Math::PointMappingCoordinateSystem::AddMappingPoint(Double mapX, Double mapY, Double baseX, Double baseY)
{
	Double *ptItem;
	ptItem = MemAlloc(Double, 4);
	ptItem[0] = mapX;
	ptItem[1] = mapY;
	ptItem[2] = baseX;
	ptItem[3] = baseY;
	this->mappingList.Add(ptItem);
}

Math::Coord2DDbl Math::PointMappingCoordinateSystem::ToBaseXY(Math::Coord2DDbl mapPt) const
{
	if (this->mappingList.GetCount() < 3)
	{
		return mapPt;
	}
	Double dist;
	Double ptDist[3];
	Double *ptList[3];
	UnsafeArray<Double> ptItem;
	UOSInt i;
	UOSInt j;
	ptDist[0] = -1;
	ptDist[1] = -1;
	ptDist[2] = -1;
	ptList[0] = 0;
	ptList[1] = 0;
	ptList[2] = 0;
	i = 0;
	j = this->mappingList.GetCount();
	while (i < j)
	{
		ptItem = this->mappingList.GetItemNoCheck(i);
		dist = (ptItem[0] - mapPt.x) * (ptItem[0] - mapPt.x) + (ptItem[1] - mapPt.y) * (ptItem[1] - mapPt.y);
		if (ptDist[0] < 0 || ptDist[0] > dist)
		{
			ptDist[2] = ptDist[1];
			ptList[2] = ptList[1];
			ptDist[1] = ptDist[0];
			ptList[1] = ptList[0];
			ptDist[0] = dist;
			ptList[0] = ptItem.Ptr();
		}
		else if (ptDist[1] < 0 || ptDist[1] > dist)
		{
			ptDist[2] = ptDist[1];
			ptList[2] = ptList[1];
			ptDist[1] = dist;
			ptList[1] = ptItem.Ptr();
		}
		else if (ptDist[2] < 0 || ptDist[2] > dist)
		{
			ptDist[2] = dist;
			ptList[2] = ptItem.Ptr();
		}
		i++;
	}
/*
	px1 py1 d1 bx1 by1
	px2 py2 d2 bx2 by2
	px3 py3 d3 bx3 by3
	d23 = dh2 + dh3
	cos 2 = h / d2
	cos 3 = h / d3
	cos 

*/
	////////////////////////////////////////////
	return mapPt;
}

Math::Coord2DDbl Math::PointMappingCoordinateSystem::ToMapXY(Math::Coord2DDbl basePt) const
{
	return basePt;
}

NN<Math::CoordinateSystem> Math::PointMappingCoordinateSystem::GetBaseCSys() const
{
	return this->baseCSys;
}

Double Math::PointMappingCoordinateSystem::CalSurfaceDistance(Math::Coord2DDbl pos1, Math::Coord2DDbl pos2, Math::Unit::Distance::DistanceUnit unit) const
{
	Math::Coord2DDbl ptList[2];
	ptList[0] = ToBaseXY(pos1);
	ptList[1] = ToBaseXY(pos2);
	return this->baseCSys->CalSurfaceDistance(ptList[0], ptList[1], unit);
}

Double Math::PointMappingCoordinateSystem::CalLineStringDistance(NN<Math::Geometry::LineString> lineString, Bool include3D, Math::Unit::Distance::DistanceUnit unit) const
{
	NN<Math::Geometry::LineString> tmpLS = NN<Math::Geometry::LineString>::ConvertFrom(lineString->Clone());
	Double ret = 0;
	UOSInt i;
	UnsafeArray<Math::Coord2DDbl> ptList = tmpLS->GetPointList(i);
	while (i-- > 0)
	{
		ptList[i] = ToBaseXY(ptList[i]);
	}
	ret = this->baseCSys->CalLineStringDistance(tmpLS, include3D, unit);
	tmpLS.Delete();
	return ret;
}

NN<Math::CoordinateSystem> Math::PointMappingCoordinateSystem::Clone() const
{
	NN<Math::PointMappingCoordinateSystem> csys;
	UOSInt i;
	UOSInt j;
	UnsafeArray<Double> ptItem;
	NEW_CLASSNN(csys, Math::PointMappingCoordinateSystem(this->sourceName, this->srid, this->csysName->ToCString(), this->baseCSys->Clone()));
	i = 0;
	j = this->mappingList.GetCount();
	while (i < j)
	{
		ptItem = this->mappingList.GetItemNoCheck(i);
		csys->AddMappingPoint(ptItem[0], ptItem[1], ptItem[2], ptItem[3]);
		i++;
	}
	return csys;
}

Math::CoordinateSystem::CoordinateSystemType Math::PointMappingCoordinateSystem::GetCoordSysType() const
{
	return Math::CoordinateSystem::CoordinateSystemType::PointMapping;
}

Bool Math::PointMappingCoordinateSystem::IsProjected() const
{
	return this->baseCSys->IsProjected();
}

void Math::PointMappingCoordinateSystem::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	UOSInt i;
	UOSInt j;
	UnsafeArray<Double> ptItem;
	sb->AppendC(UTF8STRC("Point Mapping File Name: "));
	sb->Append(this->sourceName);
	sb->AppendC(UTF8STRC("\r\nCSys Name: "));
	sb->Append(this->csysName);
	sb->AppendC(UTF8STRC("\r\nPoints:"));
	i = 0;
	j = this->mappingList.GetCount();
	while (i < j)
	{
		ptItem = this->mappingList.GetItemNoCheck(i);
		sb->AppendC(UTF8STRC("\r\n"));
		Text::SBAppendF64(sb, ptItem[0]);
		sb->AppendC(UTF8STRC(", "));
		Text::SBAppendF64(sb, ptItem[1]);
		sb->AppendC(UTF8STRC(", "));
		Text::SBAppendF64(sb, ptItem[2]);
		sb->AppendC(UTF8STRC(", "));
		Text::SBAppendF64(sb, ptItem[3]);
		i++;
	}
	sb->AppendC(UTF8STRC("\r\nBase Coordinate System:\r\n"));
	this->baseCSys->ToString(sb);
}

Bool Math::PointMappingCoordinateSystem::Equals(NN<const CoordinateSystem> csys) const
{
	if (csys->GetCoordSysType() != Math::CoordinateSystem::CoordinateSystemType::PointMapping)
	{
		return false;
	}
	const Math::PointMappingCoordinateSystem *pmcs = (const Math::PointMappingCoordinateSystem*)csys.Ptr();
	UnsafeArray<Double> ptItem1;
	UnsafeArray<Double> ptItem2;
	UOSInt i;
	UOSInt j = this->mappingList.GetCount();
	if (pmcs->mappingList.GetCount() != j)
		return false;
	i = 0;
	while (i < j)
	{
		ptItem1 = this->mappingList.GetItemNoCheck(i);
		ptItem2 = pmcs->mappingList.GetItemNoCheck(i);
		if (ptItem1[0] != ptItem2[0] || ptItem1[1] != ptItem2[1] || ptItem1[2] != ptItem2[2] || ptItem1[3] != ptItem2[3])
		{
			return false;
		}
		i++;
	}
	return this->baseCSys->Equals(pmcs->baseCSys);
}
