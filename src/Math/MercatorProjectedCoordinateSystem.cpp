#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Math/MercatorProjectedCoordinateSystem.h"

Math::MercatorProjectedCoordinateSystem::MercatorProjectedCoordinateSystem(NotNullPtr<Text::String> sourceName, UInt32 srid, Text::CString csysName, Double falseEasting, Double falseNorthing, Double centralMeridian, Double latitudeOfOrigin, Double scaleFactor, Math::GeographicCoordinateSystem *gcs, UnitType unit) : ProjectedCoordinateSystem(sourceName, srid, csysName, falseEasting, falseNorthing, centralMeridian, latitudeOfOrigin, scaleFactor, gcs, unit)
{
}

Math::MercatorProjectedCoordinateSystem::MercatorProjectedCoordinateSystem(Text::CStringNN sourceName, UInt32 srid, Text::CString csysName, Double falseEasting, Double falseNorthing, Double centralMeridian, Double latitudeOfOrigin, Double scaleFactor, Math::GeographicCoordinateSystem *gcs, UnitType unit) : ProjectedCoordinateSystem(sourceName, srid, csysName, falseEasting, falseNorthing, centralMeridian, latitudeOfOrigin, scaleFactor, gcs, unit)
{
}

Math::MercatorProjectedCoordinateSystem::~MercatorProjectedCoordinateSystem()
{
}

Math::CoordinateSystem *Math::MercatorProjectedCoordinateSystem::Clone() const
{
	Math::CoordinateSystem *csys;
 	NEW_CLASS(csys, Math::MercatorProjectedCoordinateSystem(this->sourceName, this->srid, this->csysName->ToCString(), this->falseEasting, this->falseNorthing, this->GetCentralMeridianDegree(), this->GetLatitudeOfOriginDegree(), this->scaleFactor, (Math::GeographicCoordinateSystem*)this->gcs->Clone(), this->unit));
	return csys;
}

Math::CoordinateSystem::CoordinateSystemType Math::MercatorProjectedCoordinateSystem::GetCoordSysType() const
{
	return Math::CoordinateSystem::CoordinateSystemType::MercatorProjected;
}

void Math::MercatorProjectedCoordinateSystem::ToGeographicCoordinateRad(Double projX, Double projY, Double *geoX, Double *geoY) const
{
	Math::EarthEllipsoid *ellipsoid = this->gcs->GetEllipsoid();
	Double aF = ellipsoid->GetSemiMajorAxis() * this->scaleFactor;
	Double rLatL = (projY - this->falseNorthing) / aF + this->rlatitudeOfOrigin;
	Double rLastLat;
	Double e = ellipsoid->GetEccentricity();
	Double e2 = e * e;
	Double tmpV;
	UOSInt i = 20;
	while (i-- > 0)
	{
		tmpV = projY - this->falseNorthing - this->CalcM(rLatL);
		rLastLat = rLatL;
		rLatL = rLatL + tmpV / aF;
		if (rLastLat == rLatL || (tmpV < 0.000000001 && tmpV > -0.000000001))
			break;
	}
	Double sLat = Math_Sin(rLatL);
	Double cLat = Math_Cos(rLatL);
	Double secLat = 1 / cLat;
	Double tLat = sLat * secLat; //Math_Tan(rLatL);
	Double tLat2 = tLat * tLat;
	Double tLat4 = tLat2 * tLat2;
	Double tmp = 1 - e2 * sLat * sLat;
	Double v = aF / Math_Sqrt(tmp);
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
	Double eDiff2 = eDiff * eDiff;
	Double eDiff4 = eDiff2 * eDiff2;
	Double eDiff6 = eDiff4 * eDiff2;
	*geoX = this->rcentralMeridian + ser10 * eDiff - ser11 * (eDiff2 * eDiff) + ser12 * (eDiff4 * eDiff) - ser12a * (eDiff6 * eDiff);
	*geoY = rLatL - ser7 * eDiff2 + ser8 * eDiff4 - ser9 * eDiff6;
}

void Math::MercatorProjectedCoordinateSystem::FromGeographicCoordinateRad(Double geoX, Double geoY, Double *projX, Double *projY) const
{
	Math::EarthEllipsoid *ellipsoid = this->gcs->GetEllipsoid();
	Double rLat = geoY;
	Double rLon = geoX;
	Double rLon0 = this->rcentralMeridian;
	Double sLat = Math_Sin(rLat);
	Double cLat = Math_Cos(rLat);
	Double tLat = sLat / cLat; //Math_Tan(rLat);
	Double a = ellipsoid->GetSemiMajorAxis();
	Double e = ellipsoid->GetEccentricity();
	Double e2 = e * e;
	Double tmp = 1 - e2 * sLat * sLat;
	Double v = a * this->scaleFactor / Math_Sqrt(tmp);
	Double p = v * (1 - e2) / tmp;
	Double nb2 = v / p - 1;
	Double m = this->CalcM(rLat);
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

Double Math::MercatorProjectedCoordinateSystem::CalcM(Double rLat) const
{
	Math::EarthEllipsoid *ellipsoid = this->gcs->GetEllipsoid();
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
