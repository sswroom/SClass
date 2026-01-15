#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math_C.h"
#include "Math/CassiniSoldnerCoordinateSystem.h"

Math::CassiniSoldnerCoordinateSystem::CassiniSoldnerCoordinateSystem(NN<Text::String> sourceName, UInt32 srid, Text::CStringNN csysName, Double falseEasting, Double falseNorthing, Double centralMeridian, Double latitudeOfOrigin, Double scaleFactor, NN<Math::GeographicCoordinateSystem> gcs, UnitType unit) : ProjectedCoordinateSystem(sourceName, srid, csysName, falseEasting, falseNorthing, centralMeridian, latitudeOfOrigin, scaleFactor, gcs, unit)
{
}

Math::CassiniSoldnerCoordinateSystem::CassiniSoldnerCoordinateSystem(Text::CStringNN sourceName, UInt32 srid, Text::CStringNN csysName, Double falseEasting, Double falseNorthing, Double centralMeridian, Double latitudeOfOrigin, Double scaleFactor, NN<Math::GeographicCoordinateSystem> gcs, UnitType unit) : ProjectedCoordinateSystem(sourceName, srid, csysName, falseEasting, falseNorthing, centralMeridian, latitudeOfOrigin, scaleFactor, gcs, unit)
{
}

Math::CassiniSoldnerCoordinateSystem::~CassiniSoldnerCoordinateSystem()
{
}

NN<Math::CoordinateSystem> Math::CassiniSoldnerCoordinateSystem::Clone() const
{
	NN<Math::CoordinateSystem> csys;
	NEW_CLASSNN(csys, Math::CassiniSoldnerCoordinateSystem(this->sourceName, this->srid, this->csysName->ToCString(), this->falseEasting, this->falseNorthing, this->GetCentralMeridianDegree(), this->GetLatitudeOfOriginDegree(), this->scaleFactor, NN<Math::GeographicCoordinateSystem>::ConvertFrom(this->gcs->Clone()), this->unit));
	return csys;
}

Math::CoordinateSystem::CoordinateSystemType Math::CassiniSoldnerCoordinateSystem::GetCoordSysType() const
{
	return Math::CoordinateSystem::CoordinateSystemType::CassiniSoldner;
}

Math::Coord2DDbl Math::CassiniSoldnerCoordinateSystem::ToGeographicCoordinateRad(Math::Coord2DDbl projPos) const
{
	////////////////////////////// Not correct
	NN<Math::EarthEllipsoid> ellipsoid = this->gcs->GetEllipsoid();
	Double ratio = this->GetDistanceRatio();
	Double xMeter = (projPos.x - this->falseEasting) * ratio;
	Double yMeter = (projPos.y - this->falseNorthing) * ratio;
	Double aF = ellipsoid->GetSemiMajorAxis() * this->scaleFactor;
	Double rLatL = yMeter / aF + this->rlatitudeOfOrigin;
	Double rLastLat;
	Double e = ellipsoid->GetEccentricity();
	Double e2 = e * e;
	Double tmpV;
	UIntOS i = 20;
	while (i-- > 0)
	{
		tmpV = yMeter - this->CalcM(rLatL);
		rLastLat = rLatL;
		rLatL = rLatL + tmpV / aF;
		if (rLastLat == rLatL || (tmpV < 0.000000001 && tmpV > -0.000000001))
			break;
	}
	if (rLatL >= Math::PI * 0.5)
	{
		return Math::Coord2DDbl(0, rLatL);
	}
	Double sLat = Math_Sin(rLatL);
	Double cLat = Math_Cos(rLatL);
	Double secLat = 1 / cLat;
	Double tLat = sLat * secLat; //Math_Tan(rLatL);
	Double sLat2 = sLat * sLat;
	Double T = tLat * tLat;
	Double N = Math_Pow(1 - e2 * sLat2, -0.5);
	Double R = (1 - e2) * N * N * N;
	Double D = xMeter / N;
	Double D2 = D * D;
	Double D4 = D2 * D2;
	Double lat = rLatL - (N * tLat) / R * (D2 * 0.5 - (1 + 3 * T) * D4 / 24);
	Double lon = (D - T * D2 * D / 3 + (1 + 3 * T) * D4 * D / 15) * secLat;
	return Math::Coord2DDbl(lon, lat);
}

Math::Coord2DDbl Math::CassiniSoldnerCoordinateSystem::FromGeographicCoordinateRad(Math::Coord2DDbl geoPos) const
{
	Double rLat = geoPos.GetLat() - this->rlatitudeOfOrigin;
	Double rLon = geoPos.GetLon() - this->rcentralMeridian;
	return Math::Coord2DDbl(Math_Cos(rLat) * Math_Sin(rLon) + this->falseEasting,
		this->falseNorthing + Math_ArcTan2(Math_Tan(rLat), Math_Cos(rLon))) / this->GetDistanceRatio();
}
