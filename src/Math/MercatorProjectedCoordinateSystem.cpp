#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Math/MercatorProjectedCoordinateSystem.h"

Math::MercatorProjectedCoordinateSystem::MercatorProjectedCoordinateSystem(Text::String *sourceName, UInt32 srid, const UTF8Char *csysName, Double falseEasting, Double falseNorthing, Double centralMeridian, Double latitudeOfOrigin, Double scaleFactor, Math::GeographicCoordinateSystem *gcs, UnitType unit) : ProjectedCoordinateSystem(sourceName, srid, csysName, falseEasting, falseNorthing, centralMeridian, latitudeOfOrigin, scaleFactor, gcs, unit)
{
}

Math::MercatorProjectedCoordinateSystem::MercatorProjectedCoordinateSystem(const UTF8Char *sourceName, UInt32 srid, const UTF8Char *csysName, Double falseEasting, Double falseNorthing, Double centralMeridian, Double latitudeOfOrigin, Double scaleFactor, Math::GeographicCoordinateSystem *gcs, UnitType unit) : ProjectedCoordinateSystem(sourceName, srid, csysName, falseEasting, falseNorthing, centralMeridian, latitudeOfOrigin, scaleFactor, gcs, unit)
{
}

Math::MercatorProjectedCoordinateSystem::~MercatorProjectedCoordinateSystem()
{
}

Math::CoordinateSystem *Math::MercatorProjectedCoordinateSystem::Clone()
{
	Math::CoordinateSystem *csys;
	NEW_CLASS(csys, Math::MercatorProjectedCoordinateSystem(this->sourceName, this->srid, this->csysName->v, this->falseEasting, this->falseNorthing, this->centralMeridian, this->latitudeOfOrigin, this->scaleFactor, (Math::GeographicCoordinateSystem*)this->gcs->Clone(), this->unit));
	return csys;
}

Math::CoordinateSystem::CoordinateSystemType Math::MercatorProjectedCoordinateSystem::GetCoordSysType()
{
	return Math::CoordinateSystem::CoordinateSystemType::MercatorProjected;
}

void Math::MercatorProjectedCoordinateSystem::ToGeographicCoordinate(Double projX, Double projY, Double *geoX, Double *geoY)
{
	Math::EarthEllipsoid *ellipsoid = this->gcs->GetEllipsoid();
	Double aF = ellipsoid->GetSemiMajorAxis() * this->scaleFactor;
	Double rLatL = (projY - this->falseNorthing) / aF + (this->latitudeOfOrigin * PI / 180.0);
	Double rLastLat;
	Double e = ellipsoid->GetEccentricity();
	Double e2 = e * e;
	Double tmpV;
	while (true)
	{
		tmpV = projY - this->falseNorthing - this->CalcM(rLatL);
		rLastLat = rLatL;
		rLatL = rLatL + tmpV / aF;
		if (rLastLat == rLatL || (tmpV < 0.000000001 && tmpV > -0.000000001))
			break;
	}
	Double sLat = Math::Sin(rLatL);
	Double tLat = Math::Tan(rLatL);
	Double tLat2 = tLat * tLat;
	Double tLat4 = tLat2 * tLat2;
	Double secLat = 1 / Math::Cos(rLatL);
	Double tmp = 1 - e2 * sLat * sLat;
	Double v = aF / Math::Sqrt(tmp);
	Double v2 = v * v;
	Double v3 = v * v2;
	Double v5 = v3 * v2;
	Double v7 = v5 * v2;
	Double p = v * (1 - e2) / tmp;
	Double nb2 = v / p - 1;

	Double ser7 = tLat / (2 * p * v);
	Double ser8 = tLat / (24 * p * v3) * (5 + 3 * tLat2 + nb2 - 9 * tLat2 * nb2);
	Double ser9 = tLat / (720 * p * v5) * (61 + 90 * tLat2 + 45 * tLat4);
	Double ser10 = secLat / v;
	Double ser11 = secLat / (6 * v3) * (v / p + 2 * tLat2);
	Double ser12 = secLat / (120 * v5) * (5 + 28 * tLat2 + 24 * tLat4);
	Double ser12a = secLat / (5040 * v7) * (61 + 662 * tLat2 + 1320 * tLat4 + 720 * tLat4 * tLat2);

	Double eDiff = projX - this->falseEasting;
	Double outX = (this->centralMeridian * PI / 180.0) + ser10 * eDiff - ser11 * Math::Pow(eDiff, 3) + ser12 * Math::Pow(eDiff, 5) - ser12a * Math::Pow(eDiff, 7);
	Double outY = rLatL - ser7 * Math::Pow(eDiff, 2) + ser8 * Math::Pow(eDiff, 4) - ser9 * Math::Pow(eDiff, 6);

	*geoX = (outX * 180.0 / PI);
	*geoY = (outY * 180.0 / PI);
}

void Math::MercatorProjectedCoordinateSystem::FromGeographicCoordinate(Double geoX, Double geoY, Double *projX, Double *projY)
{
	Math::EarthEllipsoid *ellipsoid = this->gcs->GetEllipsoid();
	Double rLat = geoY * PI / 180.0;
	Double rLon = geoX * PI / 180.0;
	Double rLon0 = this->centralMeridian * PI / 180;
	Double sLat = Math::Sin(rLat);
	Double a = ellipsoid->GetSemiMajorAxis();
	Double e = ellipsoid->GetEccentricity();
	Double e2 = e * e;
	Double tmp = 1 - e2 * sLat * sLat;
	Double v = a * this->scaleFactor / Math::Sqrt(tmp);
	Double p = v * (1 - e2) / tmp;
	Double nb2 = v / p - 1;
	Double m = this->CalcM(rLat);
	Double cLat = Math::Cos(rLat);
	Double tLat = Math::Tan(rLat);
	Double tLat2 = tLat * tLat;
	Double tLat4 = tLat2 * tLat2;
	Double cLat3 = cLat * cLat * cLat;
	Double cLat5 = cLat3 * cLat * cLat;
	
	Double ser1 = m + this->falseNorthing;
	Double ser2 = v * 0.5 * cLat * sLat;
	Double ser3 = v / 24 * sLat * cLat3 * (5 - tLat2 + 9 * nb2);
	Double ser3a = v / 720 * sLat * cLat5 * (61 - 58 * tLat2 + tLat4);
	Double ser4 = v * cLat;
	Double ser5 = v / 6 * cLat3 * (v / p - tLat2);
	Double ser6 = v / 120 * cLat5 * (5 - 18 * tLat2 + tLat4 + 14 * nb2 - 58 * tLat2 * nb2);
	Double dlon = rLon - rLon0;
	Double dlon2 = dlon * dlon;
	Double dlon4 = dlon2 * dlon2;

	*projX = this->falseEasting + ser4 * dlon + ser5 * dlon * dlon2 + ser6 * dlon * dlon4;
	*projY = ser1 + ser2 * dlon2 + ser3 * dlon4 + ser3a * dlon4 * dlon2;
}

Double Math::MercatorProjectedCoordinateSystem::CalcM(Double rLat)
{
	Math::EarthEllipsoid *ellipsoid = this->gcs->GetEllipsoid();
	Double a = ellipsoid->GetSemiMajorAxis();
	Double b = ellipsoid->GetSemiMinorAxis();
	Double n = (a - b) / (a + b);
	Double n2 = n * n;
	Double n3 = n2 * n;
	Double rLat0 = this->latitudeOfOrigin * PI / 180;
	Double m;
	m = (1 + n + 1.25 * n2 + 1.25 * n3) * (rLat - rLat0);
	m = m - (3 * n + 3 * n2  + 2.625 * n3) * Math::Sin(rLat - rLat0) * Math::Cos(rLat + rLat0);
	m = m + (1.875 * n2 + 1.875 * n3) * Math::Sin(2 * (rLat - rLat0)) * Math::Cos(2 * (rLat + rLat0));
	m = m - 35 / 24 * n3 * Math::Sin(3 * (rLat - rLat0)) * Math::Cos(3 * (rLat + rLat0));
	m = m * b * this->scaleFactor;
	return m;
}
