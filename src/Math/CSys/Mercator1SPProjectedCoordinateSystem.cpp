#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math_C.h"
#include "Math/CSys/Mercator1SPProjectedCoordinateSystem.h"

Math::CSys::Mercator1SPProjectedCoordinateSystem::Mercator1SPProjectedCoordinateSystem(NN<Text::String> sourceName, UInt32 srid, Text::CStringNN csysName, Double falseEasting, Double falseNorthing, Double centralMeridian, Double latitudeOfOrigin, Double scaleFactor, NN<GeographicCoordinateSystem> gcs, UnitType unit) : ProjectedCoordinateSystem(sourceName, srid, csysName, falseEasting, falseNorthing, centralMeridian, latitudeOfOrigin, scaleFactor, gcs, unit)
{
}

Math::CSys::Mercator1SPProjectedCoordinateSystem::Mercator1SPProjectedCoordinateSystem(Text::CStringNN sourceName, UInt32 srid, Text::CStringNN csysName, Double falseEasting, Double falseNorthing, Double centralMeridian, Double latitudeOfOrigin, Double scaleFactor, NN<GeographicCoordinateSystem> gcs, UnitType unit) : ProjectedCoordinateSystem(sourceName, srid, csysName, falseEasting, falseNorthing, centralMeridian, latitudeOfOrigin, scaleFactor, gcs, unit)
{
}

Math::CSys::Mercator1SPProjectedCoordinateSystem::~Mercator1SPProjectedCoordinateSystem()
{
}

NN<Math::CSys::CoordinateSystem> Math::CSys::Mercator1SPProjectedCoordinateSystem::Clone() const
{
	NN<CoordinateSystem> csys;
	NEW_CLASSNN(csys, Mercator1SPProjectedCoordinateSystem(this->sourceName, this->srid, this->csysName->ToCString(), this->falseEasting, this->falseNorthing, this->GetCentralMeridianDegree(), this->GetLatitudeOfOriginDegree(), this->scaleFactor, NN<GeographicCoordinateSystem>::ConvertFrom(this->gcs->Clone()), this->unit));
	return csys;
}

Math::CSys::CoordinateSystem::CoordinateSystemType Math::CSys::Mercator1SPProjectedCoordinateSystem::GetCoordSysType() const
{
	return CoordinateSystem::CoordinateSystemType::Mercator1SPProjected;
}

Math::Coord2DDbl Math::CSys::Mercator1SPProjectedCoordinateSystem::ToGeographicCoordinateRad(Math::Coord2DDbl projPos) const
{
	NN<Math::EarthEllipsoid> ellipsoid = this->gcs->GetEllipsoid();
	Double rLon0 = this->rcentralMeridian;
	Double a = ellipsoid->GetSemiMajorAxis();
	return Math::Coord2DDbl(((projPos.x - this->falseEasting) / a + rLon0),
		(Math_ArcTan(Math_Exp((projPos.y - this->falseNorthing) / a)) - Math::PI * 0.25) * 2);
}

Math::Coord2DDbl Math::CSys::Mercator1SPProjectedCoordinateSystem::FromGeographicCoordinateRad(Math::Coord2DDbl geoPos) const
{
	NN<Math::EarthEllipsoid> ellipsoid = this->gcs->GetEllipsoid();
	Double rLat = geoPos.GetLat();
	Double rLon = geoPos.GetLon();
	Double rLon0 = this->rcentralMeridian;
	Double a = ellipsoid->GetSemiMajorAxis();
	Double dlon = rLon - rLon0;
	return Math::Coord2DDbl(this->falseEasting + dlon * a,
		this->falseNorthing + a * Math_Ln(Math_Tan(Math::PI * 0.25 + rLat * 0.5)));
}
