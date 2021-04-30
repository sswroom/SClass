#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Math/MercatorProjectedCoordinateSystem.h"
#include "Math/Mercator1SPProjectedCoordinateSystem.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

Math::ProjectedCoordinateSystem::ProjectedCoordinateSystem(const UTF8Char *sourceName, UInt32 srid, const UTF8Char *projName, Double falseEasting, Double falseNorthing, Double centralMeridian, Double latitudeOfOrigin, Double scaleFactor, Math::GeographicCoordinateSystem *gcs, Math::CoordinateSystem::UnitType unit) : Math::CoordinateSystem(sourceName, srid, projName)
{
	this->falseEasting = falseEasting;
	this->falseNorthing = falseNorthing;
	this->centralMeridian = centralMeridian;
	this->latitudeOfOrigin = latitudeOfOrigin;
	this->scaleFactor = scaleFactor;
	this->gcs = gcs;
	this->unit = unit;
}

Math::ProjectedCoordinateSystem::~ProjectedCoordinateSystem()
{
	SDEL_CLASS(this->gcs);
}

Double Math::ProjectedCoordinateSystem::CalSurfaceDistanceXY(Double x1, Double y1, Double x2, Double y2, Math::Unit::Distance::DistanceUnit unit)
{
	Double xDiff = x2 - x1;
	Double yDiff = y2 - y1;
	Double d = Math::Sqrt(xDiff * xDiff + yDiff * yDiff);
	if (unit != Math::Unit::Distance::DU_METER)
	{
		d = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_METER, unit, d);
	}
	return d;
}

Double Math::ProjectedCoordinateSystem::CalPLDistance(Math::Polyline *pl, Math::Unit::Distance::DistanceUnit unit)
{
	UOSInt nPoint;
	UOSInt nPtOfst;
	UInt32 *ptOfsts;
	Double *points;
	ptOfsts = pl->GetPtOfstList(&nPtOfst);
	points = pl->GetPointList(&nPoint);
	UOSInt i = nPtOfst;
	UOSInt j = nPoint;
	UOSInt k;
	Double totalDist = 0;
	Bool hasLast;
	Double lastX;
	Double lastY;
	while (i-- > 0)
	{
		k = ptOfsts[i];
		hasLast = false;
		while (j-- > k)
		{
			if (hasLast)
			{
				totalDist += CalSurfaceDistanceXY(lastX, lastY, points[(j << 1)], points[(j << 1) + 1], unit);
			}
			hasLast = true;
			lastX = points[(j << 1)];
			lastY = points[(j << 1) + 1];
		}
		j++;
	}
	return totalDist;
}

Double Math::ProjectedCoordinateSystem::CalPLDistance3D(Math::Polyline3D *pl, Math::Unit::Distance::DistanceUnit unit)
{
	UOSInt nPoint;
	UOSInt nPtOfst;
	UOSInt nAlts;
	UInt32 *ptOfsts;
	Double *points;
	Double *alts;
	ptOfsts = pl->GetPtOfstList(&nPtOfst);
	points = pl->GetPointList(&nPoint);
	alts = pl->GetAltitudeList(&nAlts);
	UOSInt i = nPtOfst;
	UOSInt j = nPoint;
	UOSInt k;
	Double dist;
	Double totalDist = 0;
	Bool hasLast;
	Double lastX;
	Double lastY;
	Double lastH;
	while (i-- > 0)
	{
		k = ptOfsts[i];
		hasLast = false;
		while (j-- > k)
		{
			if (hasLast)
			{
				dist = CalSurfaceDistanceXY(lastX, lastY, points[(j << 1)], points[(j << 1) + 1], unit);
				dist = Math::Sqrt(dist * dist + (alts[j] - lastH) * (alts[j] - lastH));
				totalDist += dist;
			}
			hasLast = true;
			lastX = points[(j << 1)];
			lastY = points[(j << 1) + 1];
			lastH = alts[j];
		}
		j++;
	}
	return totalDist;
}

void Math::ProjectedCoordinateSystem::ToString(Text::StringBuilderUTF *sb)
{
	sb->Append((const UTF8Char*)"Projected File Name: ");
	sb->Append(this->sourceName);
	sb->Append((const UTF8Char*)"\r\nProjected Name: ");
	sb->Append(this->csysName);
	sb->Append((const UTF8Char*)"\r\nFalse Easting: ");
	Text::SBAppendF64(sb, this->falseEasting);
	sb->Append((const UTF8Char*)"\r\nFalse Northing: ");
	Text::SBAppendF64(sb, this->falseNorthing);
	sb->Append((const UTF8Char*)"\r\nCentral Meridian: ");
	Text::SBAppendF64(sb, this->centralMeridian);
	sb->Append((const UTF8Char*)"\r\nLatitude Of Origin: ");
	Text::SBAppendF64(sb, this->latitudeOfOrigin);
	sb->Append((const UTF8Char*)"\r\nScale Factor: ");
	Text::SBAppendF64(sb, this->scaleFactor);
	sb->Append((const UTF8Char*)"\r\n");
	this->gcs->ToString(sb);
}

Bool Math::ProjectedCoordinateSystem::IsProjected()
{
	return true;
}

Math::GeographicCoordinateSystem *Math::ProjectedCoordinateSystem::GetGeographicCoordinateSystem()
{
	return this->gcs;
}

Bool Math::ProjectedCoordinateSystem::SameProjection(Math::ProjectedCoordinateSystem *csys)
{
	if (this->falseEasting != csys->falseEasting)
		return false;
	if (this->falseNorthing != csys->falseNorthing)
		return false;
	if (this->centralMeridian != csys->centralMeridian)
		return false;
	if (this->latitudeOfOrigin != csys->latitudeOfOrigin)
		return false;
	if (this->scaleFactor != csys->scaleFactor)
		return false;
	return this->gcs->Equals(csys->gcs);
}

Double Math::ProjectedCoordinateSystem::GetLatitudeOfOrigin()
{
	return this->latitudeOfOrigin;
}

Double Math::ProjectedCoordinateSystem::GetCentralMeridian()
{
	return this->centralMeridian;
}

Double Math::ProjectedCoordinateSystem::GetScaleFactor()
{
	return this->scaleFactor;
}

Double Math::ProjectedCoordinateSystem::GetFalseEasting()
{
	return this->falseEasting;
}

Double Math::ProjectedCoordinateSystem::GetFalseNorthing()
{
	return this->falseNorthing;
}

Math::CoordinateSystem::UnitType Math::ProjectedCoordinateSystem::GetUnit()
{
	return this->unit;
}

const UTF8Char *Math::ProjectedCoordinateSystem::GetCoordinateSystemName(ProjCoordSysType pcst)
{
	switch (pcst)
	{
	case PCST_HK80:
		return (const UTF8Char*)"Hong Kong 1980 Grid System";
	case PCST_UK_NATIONAL_GRID:
		return (const UTF8Char*)"UK National Grid";
	case PCST_IRISH_NATIONAL_GRID:
		return (const UTF8Char*)"Irish National Grid";
	case PCST_MACAU_GRID:
		return (const UTF8Char*)"Macau Grid";
	case PCST_TWD67:
		return (const UTF8Char*)"Taiwan 1967 Grid";
	case PCST_TWD97:
		return (const UTF8Char*)"Taiwan 1997 Grid";
	}
	return 0;
}
