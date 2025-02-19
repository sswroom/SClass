#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Math/MercatorProjectedCoordinateSystem.h"
#include "Math/Mercator1SPProjectedCoordinateSystem.h"
#include "Math/Geometry/LineString.h"
#include "Math/Geometry/Polyline.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

Math::ProjectedCoordinateSystem::ProjectedCoordinateSystem(NN<Text::String> sourceName, UInt32 srid, Text::CStringNN projName, Double falseEasting, Double falseNorthing, Double dcentralMeridian, Double dlatitudeOfOrigin, Double scaleFactor, NN<Math::GeographicCoordinateSystem> gcs, Math::CoordinateSystem::UnitType unit) : Math::CoordinateSystem(sourceName, srid, projName)
{
	this->falseEasting = falseEasting;
	this->falseNorthing = falseNorthing;
	this->rcentralMeridian = dcentralMeridian * Math::PI / 180;
	this->rlatitudeOfOrigin = dlatitudeOfOrigin * Math::PI / 180;
	this->scaleFactor = scaleFactor;
	this->gcs = gcs;
	this->unit = unit;
}

Math::ProjectedCoordinateSystem::ProjectedCoordinateSystem(Text::CStringNN sourceName, UInt32 srid, Text::CStringNN projName, Double falseEasting, Double falseNorthing, Double dcentralMeridian, Double dlatitudeOfOrigin, Double scaleFactor, NN<Math::GeographicCoordinateSystem> gcs, Math::CoordinateSystem::UnitType unit) : Math::CoordinateSystem(sourceName, srid, projName)
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
	Math::Unit::Distance::DistanceUnit thisUnit = this->GetDistanceUnit();
	if (unit != thisUnit)
	{
		d = Math::Unit::Distance::Convert(thisUnit, unit, d);
	}
	return d;
}

Double Math::ProjectedCoordinateSystem::CalLineStringDistance(NN<Math::Geometry::LineString> lineString, Bool include3D, Math::Unit::Distance::DistanceUnit unit) const
{
	UOSInt nPoint;
	UOSInt nAlts;
	UnsafeArray<Math::Coord2DDbl> points;
	UnsafeArray<Double> alts;
	points = lineString->GetPointList(nPoint);
	UOSInt j = nPoint;
	Double totalDist = 0;
	Double dist;
	Math::Coord2DDbl lastPt;
	Double lastH;
	if (j == 0)
		return 0;
	if (include3D && lineString->GetZList(nAlts).SetTo(alts))
	{
		if (lineString->GetVectorType() == Math::Geometry::Vector2D::VectorType::LinearRing)
		{
			lastPt = points[0];
			lastH = alts[0];
		}
		else
		{
			j--;
			lastPt = points[j];
			lastH = alts[j];
		}
		while (j-- > 0)
		{
			dist = CalSurfaceDistance(lastPt, points[j], unit);
			dist = Math_Sqrt(dist * dist + (alts[j] - lastH) * (alts[j] - lastH));
			totalDist += dist;
			lastPt = points[j];
			lastH = alts[j];
		}
		return totalDist;
	}
	else
	{
		if (lineString->GetVectorType() == Math::Geometry::Vector2D::VectorType::LinearRing)
		{
			lastPt = points[0];
		}
		else
		{
			j--;
			lastPt = points[j];
		}
		while (j-- > 0)
		{
			totalDist += CalSurfaceDistance(lastPt, points[j], unit);
			lastPt = points[j];
		}
		return totalDist;
	}
}

Bool Math::ProjectedCoordinateSystem::IsProjected() const
{
	return true;
}

void Math::ProjectedCoordinateSystem::ToString(NN<Text::StringBuilderUTF8> sb) const
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

Math::Unit::Distance::DistanceUnit Math::ProjectedCoordinateSystem::GetDistanceUnit() const
{
	switch (this->unit)
	{
		default:
		case UT_DEGREE:
		case UT_METRE:
			return Math::Unit::Distance::DU_METER;
		case UT_CLARKE_FOOT:
			return Math::Unit::Distance::DU_CLARKE_FOOT;
	}
}

Double Math::ProjectedCoordinateSystem::GetDistanceRatio() const
{
	switch (this->unit)
	{
		case UT_DEGREE:
			return 0.0174532925199433;
		default:
		case UT_METRE:
			return 1;
		case UT_CLARKE_FOOT:
			return 0.3047972654;
	}
}

Bool Math::ProjectedCoordinateSystem::SameProjection(NN<const Math::ProjectedCoordinateSystem> csys) const
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

Double Math::ProjectedCoordinateSystem::CalcM(Double rLat) const
{
	NN<Math::EarthEllipsoid> ellipsoid = this->gcs->GetEllipsoid();
	Double a = ellipsoid->GetSemiMajorAxis();
	Double b = ellipsoid->GetSemiMinorAxis();
	Double n = (a - b) / (a + b);
	Double n2 = n * n;
	Double n3 = n2 * n;
	Double rLat0 = this->rlatitudeOfOrigin;
	Double m;
	m = (1 + n + 1.25 * n2 + 1.25 * n3) * (rLat - rLat0);
	m = m - (3 * n + 3 * n2  + 2.625 * n3) * Math_Sin(rLat - rLat0) * Math_Cos(rLat + rLat0);
	m = m + (1.875 * n2 + 1.875 * n3) * Math_Sin(2 * (rLat - rLat0)) * Math_Cos(2 * (rLat + rLat0));
	m = m - 35 / 24 * n3 * Math_Sin(3 * (rLat - rLat0)) * Math_Cos(3 * (rLat + rLat0));
	m = m * b * this->scaleFactor;
	return m;
}
