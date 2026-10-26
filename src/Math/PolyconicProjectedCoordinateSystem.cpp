#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math_C.h"
#include "Math/PolyconicProjectedCoordinateSystem.h"

Double Math::PolyconicProjectedCoordinateSystem::MeridionalArc(Double a, Double e2, Double lat)
{
	Double e4 = e2 * e2;
	Double e6 = e4 * e2;
	Double e8 = e4 * e4;
	Double a0 = 1 - e2 / 4 - 3 * e4 / 64 - 5 * e6 / 256 - 175 * e8 / 16384;
	Double a2 = 3 * e2 / 8 + 3 * e4 / 32 + 45 * e6 / 1024 + 105 * e8 / 4096;
	Double a4 = 15 * e4 / 256 + 45 * e6 / 1024 + 525 * e8 / 16384;
	Double a6 = 35 * e6 / 3072 + 175 * e8 / 12288;
	Double a8 = 315 * e8 / 131072;
	return a * (a0 * lat - a2 * Math_Sin(2 * lat) + a4 * Math_Sin(4 * lat) - a6 * Math_Sin(6 * lat) + a8 * Math_Sin(8 * lat));
}

Math::PolyconicProjectedCoordinateSystem::PolyconicProjectedCoordinateSystem(NN<Text::String> sourceName, UInt32 srid, Text::CStringNN csysName, Double falseEasting, Double falseNorthing, Double centralMeridian, Double latitudeOfOrigin, Double scaleFactor, NN<Math::GeographicCoordinateSystem> gcs, UnitType unit) : ProjectedCoordinateSystem(sourceName, srid, csysName, falseEasting, falseNorthing, centralMeridian, latitudeOfOrigin, scaleFactor, gcs, unit)
{
}

Math::PolyconicProjectedCoordinateSystem::PolyconicProjectedCoordinateSystem(Text::CStringNN sourceName, UInt32 srid, Text::CStringNN csysName, Double falseEasting, Double falseNorthing, Double centralMeridian, Double latitudeOfOrigin, Double scaleFactor, NN<Math::GeographicCoordinateSystem> gcs, UnitType unit) : ProjectedCoordinateSystem(sourceName, srid, csysName, falseEasting, falseNorthing, centralMeridian, latitudeOfOrigin, scaleFactor, gcs, unit)
{
}

Math::PolyconicProjectedCoordinateSystem::~PolyconicProjectedCoordinateSystem()
{
}

NN<Math::CoordinateSystem> Math::PolyconicProjectedCoordinateSystem::Clone() const
{
	NN<Math::CoordinateSystem> csys;
	NEW_CLASSNN(csys, Math::PolyconicProjectedCoordinateSystem(this->sourceName, this->srid, this->csysName->ToCString(), this->falseEasting, this->falseNorthing, this->GetCentralMeridianDegree(), this->GetLatitudeOfOriginDegree(), this->scaleFactor, NN<Math::GeographicCoordinateSystem>::ConvertFrom(this->gcs->Clone()), this->unit));
	return csys;
}

Math::CoordinateSystem::CoordinateSystemType Math::PolyconicProjectedCoordinateSystem::GetCoordSysType() const
{
	return Math::CoordinateSystem::CoordinateSystemType::PolyconicProjected;
}

Math::Coord2DDbl Math::PolyconicProjectedCoordinateSystem::ToGeographicCoordinateRad(Math::Coord2DDbl projPos) const
{
	NN<Math::EarthEllipsoid> ellipsoid = this->gcs->GetEllipsoid();
	Double a = ellipsoid->GetSemiMajorAxis() * this->scaleFactor;
	Double e = ellipsoid->GetEccentricity();
	Double e2 = e * e;
	Double x = projPos.x - this->falseEasting;
	Double y = projPos.y - this->falseNorthing;
	Double lat = this->rlatitudeOfOrigin + y / a;
	Double lon = this->rcentralMeridian + x / (a * Math_Cos(lat));
	Double residualX;
	Double residualY;
	UIntOS i = 30;

	while (i-- > 0)
	{
		Double sLat = Math_Sin(lat);
		Double cLat = Math_Cos(lat);
		Double dLon = lon - this->rcentralMeridian;
		Double n;
		Double c;
		Double eAng;
		Double sinE;
		Double cosE;
		Double calcX;
		Double calcY;
		Double xLat;
		Double xLon;
		Double yLat;
		Double yLon;
		Double q;
		Double det;
		Double dLat;
		Double dLongitude;

		if (lat <= -Math::PI * 0.5 || lat >= Math::PI * 0.5)
			return Math::Coord2DDbl(NAN, NAN);

		if (lat > -0.00000001 && lat < 0.00000001)
		{
			calcX = a * dLon;
			calcY = MeridionalArc(a, e2, 0) - MeridionalArc(a, e2, this->rlatitudeOfOrigin) + (a * (1 - e2) + a * dLon * dLon * 0.5) * lat;
			xLat = 0;
			xLon = a;
			yLat = a * (1 - e2) + a * dLon * dLon * 0.5;
			yLon = 0;
		}
		else
		{
			q = 1 - e2 * sLat * sLat;
			n = a / Math_Sqrt(q);
			c = n * cLat / sLat;
			eAng = dLon * sLat;
			sinE = Math_Sin(eAng);
			cosE = Math_Cos(eAng);
			calcX = c * sinE;
			calcY = MeridionalArc(a, e2, lat) - MeridionalArc(a, e2, this->rlatitudeOfOrigin) + c * 2 * Math_Sin(eAng * 0.5) * Math_Sin(eAng * 0.5);
			Double nLat = n * e2 * sLat * cLat / q;
			Double cLatDerivative = nLat * cLat / sLat - n / (sLat * sLat);
			xLat = cLatDerivative * sinE + c * cosE * dLon * cLat;
			xLon = c * cosE * sLat;
			yLat = a * (1 - e2) / (q * Math_Sqrt(q)) + cLatDerivative * (1 - cosE) + c * sinE * dLon * cLat;
			yLon = c * sinE * sLat;
		}

		residualX = calcX - x;
		residualY = calcY - y;
		if (residualX > -0.0000001 && residualX < 0.0000001 && residualY > -0.0000001 && residualY < 0.0000001)
		{
			return Math::Coord2DDbl(lon, lat);
		}

		det = xLon * yLat - xLat * yLon;
		if (det == 0 || Math::IsNAN(det))
			return Math::Coord2DDbl(NAN, NAN);
		dLongitude = (residualX * yLat - residualY * xLat) / det;
		dLat = (residualY * xLon - residualX * yLon) / det;
		lon -= dLongitude;
		lat -= dLat;
	}

	return Math::Coord2DDbl(NAN, NAN);
}

Math::Coord2DDbl Math::PolyconicProjectedCoordinateSystem::FromGeographicCoordinateRad(Math::Coord2DDbl geoPos) const
{
	NN<Math::EarthEllipsoid> ellipsoid = this->gcs->GetEllipsoid();
	Double lat = geoPos.GetLat();
	Double dLon = geoPos.GetLon() - this->rcentralMeridian;
	Double a = ellipsoid->GetSemiMajorAxis() * this->scaleFactor;
	Double e = ellipsoid->GetEccentricity();
	Double e2 = e * e;
	Double sLat = Math_Sin(lat);
	Double cLat = Math_Cos(lat);
	Double x;
	Double y;

	if (lat > -0.00000001 && lat < 0.00000001)
	{
		x = a * dLon;
		y = MeridionalArc(a, e2, 0) - MeridionalArc(a, e2, this->rlatitudeOfOrigin) + (a * (1 - e2) + a * dLon * dLon * 0.5) * lat;
	}
	else
	{
		Double n = a / Math_Sqrt(1 - e2 * sLat * sLat);
		Double c = n * cLat / sLat;
		Double eAng = dLon * sLat;
		x = c * Math_Sin(eAng);
		y = MeridionalArc(a, e2, lat) - MeridionalArc(a, e2, this->rlatitudeOfOrigin) + c * 2 * Math_Sin(eAng * 0.5) * Math_Sin(eAng * 0.5);
	}

	return Math::Coord2DDbl(this->falseEasting + x, this->falseNorthing + y);
}
