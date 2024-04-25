#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Math/MercatorProjectedCoordinateSystem.h"
#include "Math/Mercator1SPProjectedCoordinateSystem.h"
#include "Math/Geometry/LineString.h"
#include "Math/Geometry/Polyline.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

Math::ProjectedCoordinateSystem::ProjectedCoordinateSystem(NN<Text::String> sourceName, UInt32 srid, Text::CString projName, Double falseEasting, Double falseNorthing, Double dcentralMeridian, Double dlatitudeOfOrigin, Double scaleFactor, NN<Math::GeographicCoordinateSystem> gcs, Math::CoordinateSystem::UnitType unit) : Math::CoordinateSystem(sourceName, srid, projName)
{
	this->falseEasting = falseEasting;
	this->falseNorthing = falseNorthing;
	this->rcentralMeridian = dcentralMeridian * Math::PI / 180;
	this->rlatitudeOfOrigin = dlatitudeOfOrigin * Math::PI / 180;
	this->scaleFactor = scaleFactor;
	this->gcs = gcs;
	this->unit = unit;
}

Math::ProjectedCoordinateSystem::ProjectedCoordinateSystem(Text::CStringNN sourceName, UInt32 srid, Text::CString projName, Double falseEasting, Double falseNorthing, Double dcentralMeridian, Double dlatitudeOfOrigin, Double scaleFactor, NN<Math::GeographicCoordinateSystem> gcs, Math::CoordinateSystem::UnitType unit) : Math::CoordinateSystem(sourceName, srid, projName)
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

Double Math::ProjectedCoordinateSystem::CalLineStringDistance(NN<Math::Geometry::LineString> lineString, Bool include3D, Math::Unit::Distance::DistanceUnit unit) const
{
	UOSInt nPoint;
	UOSInt nAlts;
	Math::Coord2DDbl *points;
	Double *alts;
	points = lineString->GetPointList(nPoint);
	UOSInt j = nPoint;
	Double totalDist = 0;
	Double dist;
	Math::Coord2DDbl lastPt;
	Double lastH;
	if (j == 0)
		return 0;
	if (include3D && (alts = lineString->GetZList(nAlts)) != 0)
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
