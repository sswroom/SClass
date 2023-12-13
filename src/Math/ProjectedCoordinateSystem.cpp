#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Math/MercatorProjectedCoordinateSystem.h"
#include "Math/Mercator1SPProjectedCoordinateSystem.h"
#include "Math/Geometry/LineString.h"
#include "Math/Geometry/Polyline.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

Math::ProjectedCoordinateSystem::ProjectedCoordinateSystem(NotNullPtr<Text::String> sourceName, UInt32 srid, Text::CString projName, Double falseEasting, Double falseNorthing, Double dcentralMeridian, Double dlatitudeOfOrigin, Double scaleFactor, NotNullPtr<Math::GeographicCoordinateSystem> gcs, Math::CoordinateSystem::UnitType unit) : Math::CoordinateSystem(sourceName, srid, projName)
{
	this->falseEasting = falseEasting;
	this->falseNorthing = falseNorthing;
	this->rcentralMeridian = dcentralMeridian * Math::PI / 180;
	this->rlatitudeOfOrigin = dlatitudeOfOrigin * Math::PI / 180;
	this->scaleFactor = scaleFactor;
	this->gcs = gcs;
	this->unit = unit;
}

Math::ProjectedCoordinateSystem::ProjectedCoordinateSystem(Text::CStringNN sourceName, UInt32 srid, Text::CString projName, Double falseEasting, Double falseNorthing, Double dcentralMeridian, Double dlatitudeOfOrigin, Double scaleFactor, NotNullPtr<Math::GeographicCoordinateSystem> gcs, Math::CoordinateSystem::UnitType unit) : Math::CoordinateSystem(sourceName, srid, projName)
{
	this->falseEasting = falseEasting;
	this->falseNorthing = falseNorthing;
	this->rcentralMeridian = dcentralMeridian * Math::PI / 180;
	this->rlatitudeOfOrigin = dlatitudeOfOrigin * Math::PI / 180;
	this->scaleFactor = scaleFactor;
	this->gcs = gcs;
	this->unit = unit;
}

Math::ProjectedCoordinateSystem::~ProjectedCoordinateSystem()
{
	this->gcs.Delete();
}

Double Math::ProjectedCoordinateSystem::CalSurfaceDistance(Math::Coord2DDbl pos1, Math::Coord2DDbl pos2, Math::Unit::Distance::DistanceUnit unit) const
{
	Math::Coord2DDbl diff = pos2 - pos1;
	diff = diff * diff;
	Double d = Math_Sqrt(diff.x + diff.y);
	if (unit != Math::Unit::Distance::DU_METER)
	{
		d = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_METER, unit, d);
	}
	return d;
}

Double Math::ProjectedCoordinateSystem::CalLineStringDistance(NotNullPtr<Math::Geometry::LineString> lineString, Math::Unit::Distance::DistanceUnit unit) const
{
	UOSInt nPoint;
	Math::Coord2DDbl *points;
	points = lineString->GetPointList(nPoint);
	UOSInt j = nPoint;
	Double totalDist = 0;
	Bool hasLast;
	Math::Coord2DDbl lastPt;
	hasLast = false;
	while (j-- > 0)
	{
		if (hasLast)
		{
			totalDist += CalSurfaceDistance(lastPt, points[j], unit);
		}
		hasLast = true;
		lastPt = points[j];
	}
	return totalDist;
}

Double Math::ProjectedCoordinateSystem::CalLineStringDistance3D(NotNullPtr<Math::Geometry::LineString> lineString, Math::Unit::Distance::DistanceUnit unit) const
{
	UOSInt nPoint;
	UOSInt nAlts;
	Math::Coord2DDbl *points;
	Double *alts;
	points = lineString->GetPointList(nPoint);
	alts = lineString->GetZList(nAlts);
	UOSInt j = nPoint;
	Double dist;
	Double totalDist = 0;
	Bool hasLast;
	Math::Coord2DDbl lastPt;
	Double lastH;
	hasLast = false;
	while (j-- > 0)
	{
		if (hasLast)
		{
			dist = CalSurfaceDistance(lastPt, points[j], unit);
			dist = Math_Sqrt(dist * dist + (alts[j] - lastH) * (alts[j] - lastH));
			totalDist += dist;
		}
		hasLast = true;
		lastPt = points[j];
		lastH = alts[j];
	}
	return totalDist;
}

Bool Math::ProjectedCoordinateSystem::IsProjected() const
{
	return true;
}

void Math::ProjectedCoordinateSystem::ToString(NotNullPtr<Text::StringBuilderUTF8> sb) const
{
	sb->AppendC(UTF8STRC("Projected File Name: "));
	sb->Append(this->sourceName);
	sb->AppendC(UTF8STRC("\r\nProjected SRID: "));
	sb->AppendU32(this->srid);
	sb->AppendC(UTF8STRC("\r\nProjected Name: "));
	sb->Append(this->csysName);
	sb->AppendC(UTF8STRC("\r\nProjection Type: "));
	sb->Append(CoordinateSystemTypeGetName(this->GetCoordSysType()));
	sb->AppendC(UTF8STRC("\r\nFalse Easting: "));
	Text::SBAppendF64(sb, this->falseEasting);
	sb->AppendC(UTF8STRC("\r\nFalse Northing: "));
	Text::SBAppendF64(sb, this->falseNorthing);
	sb->AppendC(UTF8STRC("\r\nCentral Meridian: "));
	Text::SBAppendF64(sb, this->GetCentralMeridianDegree());
	sb->AppendC(UTF8STRC("\r\nLatitude Of Origin: "));
	Text::SBAppendF64(sb, this->GetLatitudeOfOriginDegree());
	sb->AppendC(UTF8STRC("\r\nScale Factor: "));
	Text::SBAppendF64(sb, this->scaleFactor);
	sb->AppendC(UTF8STRC("\r\n"));
	this->gcs->ToString(sb);
}

Bool Math::ProjectedCoordinateSystem::SameProjection(NotNullPtr<const Math::ProjectedCoordinateSystem> csys) const
{
	if (this->falseEasting != csys->falseEasting)
		return false;
	if (this->falseNorthing != csys->falseNorthing)
		return false;
	if (this->rcentralMeridian != csys->rcentralMeridian)
		return false;
	if (this->rlatitudeOfOrigin != csys->rlatitudeOfOrigin)
		return false;
	if (this->scaleFactor != csys->scaleFactor)
		return false;
	return this->gcs->Equals(csys->gcs);
}
