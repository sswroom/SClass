#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Math/Mercator1SPProjectedCoordinateSystem.h"

Math::Mercator1SPProjectedCoordinateSystem::Mercator1SPProjectedCoordinateSystem(Text::String *sourceName, UInt32 srid, const UTF8Char *csysName, Double falseEasting, Double falseNorthing, Double centralMeridian, Double latitudeOfOrigin, Double scaleFactor, Math::GeographicCoordinateSystem *gcs, UnitType unit) : ProjectedCoordinateSystem(sourceName, srid, csysName, falseEasting, falseNorthing, centralMeridian, latitudeOfOrigin, scaleFactor, gcs, unit)
{
}

Math::Mercator1SPProjectedCoordinateSystem::Mercator1SPProjectedCoordinateSystem(const UTF8Char *sourceName, UInt32 srid, const UTF8Char *csysName, Double falseEasting, Double falseNorthing, Double centralMeridian, Double latitudeOfOrigin, Double scaleFactor, Math::GeographicCoordinateSystem *gcs, UnitType unit) : ProjectedCoordinateSystem(sourceName, srid, csysName, falseEasting, falseNorthing, centralMeridian, latitudeOfOrigin, scaleFactor, gcs, unit)
{
}

Math::Mercator1SPProjectedCoordinateSystem::~Mercator1SPProjectedCoordinateSystem()
{
}

Math::CoordinateSystem *Math::Mercator1SPProjectedCoordinateSystem::Clone()
{
	Math::CoordinateSystem *csys;
	NEW_CLASS(csys, Math::Mercator1SPProjectedCoordinateSystem(this->sourceName, this->srid, this->csysName->v, this->falseEasting, this->falseNorthing, this->centralMeridian, this->latitudeOfOrigin, this->scaleFactor, (Math::GeographicCoordinateSystem*)this->gcs->Clone(), this->unit));
	return csys;
}

Math::CoordinateSystem::CoordinateSystemType Math::Mercator1SPProjectedCoordinateSystem::GetCoordSysType()
{
	return Math::CoordinateSystem::CoordinateSystemType::Mercator1SPProjected;
}

void Math::Mercator1SPProjectedCoordinateSystem::ToGeographicCoordinate(Double projX, Double projY, Double *geoX, Double *geoY)
{
	Math::EarthEllipsoid *ellipsoid = this->gcs->GetEllipsoid();
	Double rLon0 = this->centralMeridian * PI / 180;
	Double a = ellipsoid->GetSemiMajorAxis();
	*geoX = ((projX - this->falseEasting) / a + rLon0) * 180.0 / Math::PI;
	*geoY = (Math_ArcTan(Math_Exp((projY - this->falseNorthing) / a)) - Math::PI * 0.25) * 2 * 180.0 / Math::PI;
}

void Math::Mercator1SPProjectedCoordinateSystem::FromGeographicCoordinate(Double geoX, Double geoY, Double *projX, Double *projY)
{
	Math::EarthEllipsoid *ellipsoid = this->gcs->GetEllipsoid();
	Double rLat = geoY * Math::PI / 180.0;
	Double rLon = geoX * Math::PI / 180.0;
	Double rLon0 = this->centralMeridian * Math::PI / 180;
	Double a = ellipsoid->GetSemiMajorAxis();
	Double dlon = rLon - rLon0;
	*projX = this->falseEasting + dlon * a;
	*projY = this->falseNorthing + a * Math_Ln(Math_Tan(Math::PI * 0.25 + rLat * 0.5));
}
