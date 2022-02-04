#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Math/MercatorProjectedCoordinateSystem.h"
#include "Math/Mercator1SPProjectedCoordinateSystem.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

Math::ProjectedCoordinateSystem::ProjectedCoordinateSystem(Text::String *sourceName, UInt32 srid, const UTF8Char *projName, Double falseEasting, Double falseNorthing, Double centralMeridian, Double latitudeOfOrigin, Double scaleFactor, Math::GeographicCoordinateSystem *gcs, Math::CoordinateSystem::UnitType unit) : Math::CoordinateSystem(sourceName, srid, projName)
{
	this->falseEasting = falseEasting;
	this->falseNorthing = falseNorthing;
	this->centralMeridian = centralMeridian;
	this->latitudeOfOrigin = latitudeOfOrigin;
	this->scaleFactor = scaleFactor;
	this->gcs = gcs;
	this->unit = unit;
}

Math::ProjectedCoordinateSystem::ProjectedCoordinateSystem(Text::CString sourceName, UInt32 srid, const UTF8Char *projName, Double falseEasting, Double falseNorthing, Double centralMeridian, Double latitudeOfOrigin, Double scaleFactor, Math::GeographicCoordinateSystem *gcs, Math::CoordinateSystem::UnitType unit) : Math::CoordinateSystem(sourceName, srid, projName)
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
	Double d = Math_Sqrt(xDiff * xDiff + yDiff * yDiff);
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
				dist = Math_Sqrt(dist * dist + (alts[j] - lastH) * (alts[j] - lastH));
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

Bool Math::ProjectedCoordinateSystem::IsProjected()
{
	return true;
}

void Math::ProjectedCoordinateSystem::ToString(Text::StringBuilderUTF8 *sb)
{
	sb->AppendC(UTF8STRC("Projected File Name: "));
	sb->Append(this->sourceName);
	sb->AppendC(UTF8STRC("\r\nProjected Name: "));
	sb->Append(this->csysName);
	sb->AppendC(UTF8STRC("\r\nFalse Easting: "));
	Text::SBAppendF64(sb, this->falseEasting);
	sb->AppendC(UTF8STRC("\r\nFalse Northing: "));
	Text::SBAppendF64(sb, this->falseNorthing);
	sb->AppendC(UTF8STRC("\r\nCentral Meridian: "));
	Text::SBAppendF64(sb, this->centralMeridian);
	sb->AppendC(UTF8STRC("\r\nLatitude Of Origin: "));
	Text::SBAppendF64(sb, this->latitudeOfOrigin);
	sb->AppendC(UTF8STRC("\r\nScale Factor: "));
	Text::SBAppendF64(sb, this->scaleFactor);
	sb->AppendC(UTF8STRC("\r\n"));
	this->gcs->ToString(sb);
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
