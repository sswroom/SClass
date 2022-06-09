#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Math/PointMappingCoordinateSystem.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

Math::PointMappingCoordinateSystem::PointMappingCoordinateSystem(Text::String *sourceName, UInt32 srid, Text::CString csysName, Math::CoordinateSystem *baseCSys) : Math::CoordinateSystem(sourceName, srid, csysName)
{
	this->baseCSys = baseCSys;
	NEW_CLASS(this->mappingList, Data::ArrayList<Double*>());
}

Math::PointMappingCoordinateSystem::PointMappingCoordinateSystem(Text::CString sourceName, UInt32 srid, Text::CString csysName, Math::CoordinateSystem *baseCSys) : Math::CoordinateSystem(sourceName, srid, csysName)
{
	this->baseCSys = baseCSys;
	NEW_CLASS(this->mappingList, Data::ArrayList<Double*>());
}

Math::PointMappingCoordinateSystem::~PointMappingCoordinateSystem()
{
	UOSInt i;
	Double *ptItem;
	i = this->mappingList->GetCount();
	while (i-- > 0)
	{
		ptItem = this->mappingList->GetItem(i);
		MemFree(ptItem);
	}
	DEL_CLASS(this->mappingList);
	DEL_CLASS(this->baseCSys);
}

void Math::PointMappingCoordinateSystem::AddMappingPoint(Double mapX, Double mapY, Double baseX, Double baseY)
{
	Double *ptItem;
	ptItem = MemAlloc(Double, 4);
	ptItem[0] = mapX;
	ptItem[1] = mapY;
	ptItem[2] = baseX;
	ptItem[3] = baseY;
	this->mappingList->Add(ptItem);
}

Math::Coord2DDbl Math::PointMappingCoordinateSystem::CalcBaseXY(Math::Coord2DDbl mapPt)
{
	if (this->mappingList->GetCount() < 3)
	{
		return mapPt;
	}
	Double dist;
	Double ptDist[3];
	Double *ptList[3];
	Double *ptItem;
	UOSInt i;
	UOSInt j;
	ptDist[0] = -1;
	ptDist[1] = -1;
	ptDist[2] = -1;
	ptList[0] = 0;
	ptList[1] = 0;
	ptList[2] = 0;
	i = 0;
	j = this->mappingList->GetCount();
	while (i < j)
	{
		ptItem = this->mappingList->GetItem(i);
		dist = (ptItem[0] - mapPt.x) * (ptItem[0] - mapPt.x) + (ptItem[1] - mapPt.y) * (ptItem[1] - mapPt.y);
		if (ptDist[0] < 0 || ptDist[0] > dist)
		{
			ptDist[2] = ptDist[1];
			ptList[2] = ptList[1];
			ptDist[1] = ptDist[0];
			ptList[1] = ptList[0];
			ptDist[0] = dist;
			ptList[0] = ptItem;
		}
		else if (ptDist[1] < 0 || ptDist[1] > dist)
		{
			ptDist[2] = ptDist[1];
			ptList[2] = ptList[1];
			ptDist[1] = dist;
			ptList[1] = ptItem;
		}
		else if (ptDist[2] < 0 || ptDist[2] > dist)
		{
			ptDist[2] = dist;
			ptList[2] = ptItem;
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

Double Math::PointMappingCoordinateSystem::CalSurfaceDistanceXY(Math::Coord2DDbl pos1, Math::Coord2DDbl pos2, Math::Unit::Distance::DistanceUnit unit)
{
	Math::Coord2DDbl ptList[2];
	ptList[0] = CalcBaseXY(pos1);
	ptList[1] = CalcBaseXY(pos2);
	return this->baseCSys->CalSurfaceDistanceXY(ptList[0], ptList[1], unit);
}

Double Math::PointMappingCoordinateSystem::CalPLDistance(Math::Polyline *pl, Math::Unit::Distance::DistanceUnit unit)
{
	Math::Polyline *tmpPl = (Math::Polyline*)pl->Clone();
	Double ret;
	UOSInt i;
	Math::Coord2DDbl *ptList = tmpPl->GetPointList(&i);
	while (i-- > 0)
	{
		ptList[i] = CalcBaseXY(ptList[i]);
	}
	ret = this->baseCSys->CalPLDistance(tmpPl, unit);
	DEL_CLASS(tmpPl);
	return ret;
}

Double Math::PointMappingCoordinateSystem::CalPLDistance3D(Math::Polyline3D *pl, Math::Unit::Distance::DistanceUnit unit)
{
	Math::Polyline3D *tmpPl = (Math::Polyline3D *)pl->Clone();
	Double ret;
	UOSInt i;
	Math::Coord2DDbl *ptList = tmpPl->GetPointList(&i);
	while (i-- > 0)
	{
		ptList[i] = CalcBaseXY(ptList[i]);
	}
	ret = this->baseCSys->CalPLDistance3D(tmpPl, unit);
	DEL_CLASS(tmpPl);
	return ret;
}

Math::CoordinateSystem *Math::PointMappingCoordinateSystem::Clone()
{
	Math::PointMappingCoordinateSystem *csys;
	UOSInt i;
	UOSInt j;
	Double *ptItem;
	NEW_CLASS(csys, Math::PointMappingCoordinateSystem(this->sourceName, this->srid, this->csysName->ToCString(), this->baseCSys->Clone()));
	i = 0;
	j = this->mappingList->GetCount();
	while (i < j)
	{
		ptItem = this->mappingList->GetItem(i);
		csys->AddMappingPoint(ptItem[0], ptItem[1], ptItem[2], ptItem[3]);
		i++;
	}
	return csys;
}

Math::CoordinateSystem::CoordinateSystemType Math::PointMappingCoordinateSystem::GetCoordSysType()
{
	return Math::CoordinateSystem::CoordinateSystemType::PointMapping;
}

Bool Math::PointMappingCoordinateSystem::IsProjected()
{
	return this->baseCSys->IsProjected();
}

void Math::PointMappingCoordinateSystem::ToString(Text::StringBuilderUTF8 *sb)
{
	UOSInt i;
	UOSInt j;
	Double *ptItem;
	sb->AppendC(UTF8STRC("Point Mapping File Name: "));
	sb->Append(this->sourceName);
	sb->AppendC(UTF8STRC("\r\nCSys Name: "));
	sb->Append(this->csysName);
	sb->AppendC(UTF8STRC("\r\nPoints:"));
	i = 0;
	j = this->mappingList->GetCount();
	while (i < j)
	{
		ptItem = this->mappingList->GetItem(i);
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

Bool Math::PointMappingCoordinateSystem::Equals(CoordinateSystem *csys)
{
	if (csys->GetCoordSysType() != Math::CoordinateSystem::CoordinateSystemType::PointMapping)
	{
		return false;
	}
	Math::PointMappingCoordinateSystem *pmcs = (Math::PointMappingCoordinateSystem*)csys;
	Double *ptItem1;
	Double *ptItem2;
	UOSInt i;
	UOSInt j = this->mappingList->GetCount();
	if (pmcs->mappingList->GetCount() != j)
		return false;
	i = 0;
	while (i < j)
	{
		ptItem1 = this->mappingList->GetItem(i);
		ptItem2 = pmcs->mappingList->GetItem(i);
		if (ptItem1[0] != ptItem2[0] || ptItem1[1] != ptItem2[1] || ptItem1[2] != ptItem2[2] || ptItem1[3] != ptItem2[3])
		{
			return false;
		}
		i++;
	}
	return this->baseCSys->Equals(pmcs->baseCSys);
}
