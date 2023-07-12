#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Math/Mercator1SPProjectedCoordinateSystem.h"

Math::Mercator1SPProjectedCoordinateSystem::Mercator1SPProjectedCoordinateSystem(Text::String *sourceName, UInt32 srid, Text::CString csysName, Double falseEasting, Double falseNorthing, Double centralMeridian, Double latitudeOfOrigin, Double scaleFactor, Math::GeographicCoordinateSystem *gcs, UnitType unit) : ProjectedCoordinateSystem(sourceName, srid, csysName, falseEasting, falseNorthing, centralMeridian, latitudeOfOrigin, scaleFactor, gcs, unit)
{
}

Math::Mercator1SPProjectedCoordinateSystem::Mercator1SPProjectedCoordinateSystem(Text::CString sourceName, UInt32 srid, Text::CString csysName, Double falseEasting, Double falseNorthing, Double centralMeridian, Double latitudeOfOrigin, Double scaleFactor, Math::GeographicCoordinateSystem *gcs, UnitType unit) : ProjectedCoordinateSystem(sourceName, srid, csysName, falseEasting, falseNorthing, centralMeridian, latitudeOfOrigin, scaleFactor, gcs, unit)
{
}

Math::Mercator1SPProjectedCoordinateSystem::~Mercator1SPProjectedCoordinateSystem()
{
}

Math::CoordinateSystem *Math::Mercator1SPProjectedCoordinateSystem::Clone() const
{
	Math::CoordinateSystem *csys;
	NEW_CLASS(csys, Math::Mercator1SPProjectedCoordinateSystem(this->sourceName, this->srid, this->csysName->ToCString(), this->falseEasting, this->falseNorthing, this->GetCentralMeridianDegree(), this->GetLatitudeOfOriginDegree(), this->scaleFactor, (Math::GeographicCoordinateSystem*)this->gcs->Clone(), this->unit));
	return csys;
}

Math::CoordinateSystem::CoordinateSystemType Math::Mercator1SPProjectedCoordinateSystem::GetCoordSysType() const
{
	return Math::CoordinateSystem::CoordinateSystemType::Mercator1SPProjected;
}

void Math::Mercator1SPProjectedCoordinateSystem::ToGeographicCoordinateRad(Double projX, Double projY, Double *geoX, Double *geoY) const
{
	Math::EarthEllipsoid *ellipsoid = this->gcs->GetEllipsoid();
	Double rLon0 = this->rcentralMeridian;
	Double a = ellipsoid->GetSemiMajorAxis();
	*geoX = ((projX - this->falseEasting) / a + rLon0);
	*geoY = (Math_ArcTan(Math_Exp((projY - this->falseNorthing) / a)) - Math::PI * 0.25) * 2;
}

void Math::Mercator1SPProjectedCoordinateSystem::FromGeographicCoordinateRad(Double geoX, Double geoY, Double *projX, Double *projY) const
{
	Math::EarthEllipsoid *ellipsoid = this->gcs->GetEllipsoid();
	Double rLat = geoY;
	Double rLon = geoX;
	Double rLon0 = this->rcentralMeridian;
	Double a = ellipsoid->GetSemiMajorAxis();
	Double dlon = rLon - rLon0;
	*projX = this->falseEasting + dlon * a;
	*projY = this->falseNorthing + a * Math_Ln(Math_Tan(Math::PI * 0.25 + rLat * 0.5));
}
