#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Math/EarthEllipsoid.h"

Math::EarthEllipsoid::EarthEllipsoidInfo Math::EarthEllipsoid::refEllipsoids[] = {
	{EET_OTHER,			"Other",					1984, 6378137.0,	191.0},
	{EET_PLESSIS,		"Plessis",					1817, 6376523.0,	308.64},
	{EET_EVEREST1830,	"Everest",					1830, 6377299.365,	300.80172554},
	{EET_EVEREST1830M,	"Everest 1830 Modified",	1967, 6377304.063,	300.8017},
	{EET_EVEREST1830N,	"Everest 1830 (1967 def)",	1967, 6377298.556,	300.8017},
	{EET_AIRY1830,		"Airy",						1830, 6377563.396,	299.3249646},
	{EET_AIRY1830M,		"Airy (Modified)",			1830, 6377340.189,	299.32495141450600500090538973015},
	{EET_BESSEL1841,	"Bessel",					1841, 6377397.155,	299.1528128},
	{EET_CLARKE1866,	"Clarke",					1866, 6378206.4,	294.9786982},
	{EET_CLARKE1878,	"Clarke",					1878, 6378190.0,	293.4659980},
	{EET_CLARKE1880,	"Clarke 1880 (RGS)",		1880, 6378249.145,	293.465},
	{EET_HELMERT1906,	"Helmert",					1906, 6378200.0,	298.3},
	{EET_HAYFORD1910,	"Hayford",					1910, 6378388.0,	297.0},
	{EET_INTL1924,		"International",			1924, 6378388.0,	297.0},
	{EET_KRASSOVSKY1940, "Krassovsky",				1940, 6378245.0,	298.3},
	{EET_WGS66,			"WGS66",					1966, 6378145.0,	298.25},
	{EET_AUSTRALIAN1966, "Australian National",		1966, 6378160.0,	298.25},
	{EET_NEWINTL1967,	"New International",		1967, 6378157.5,	298.24961539},
	{EET_GPS67,			"GPS-67",					1967, 6378160.0,	298.247167427},
	{EET_SAM1969,		"South American",			1969, 6378160.0,	298.25},
	{EET_WGS72,			"WGS-72",					1972, 6378135.0,	298.26},
	{EET_GRS80,			"GRS-80",					1979, 6378137.0,	298.257222101},
	{EET_WGS84,			"WGS-84",					1984, 6378137.0,	298.257223563},
	{EET_WGS84_OGC,		"WGS-84 (OGC Values)",		1984, 6378137.0,	298.257222932867},
	{EET_IERS1989,		"IERS",						1989, 6378136.0,	298.257},
	{EET_IERS2003,		"IERS",						2003, 6378136.6,	298.25642}
};

Math::EarthEllipsoid::EarthEllipsoid(Double semiMajorAxis, Double inverseFlattening, EarthEllipsoidType eet)
{
	this->eet = eet;
	this->semiMajorAxis = semiMajorAxis;
	this->inverseFlattening = inverseFlattening;
	this->semiMinorAxis = this->semiMajorAxis * (1.0 - 1.0 / this->inverseFlattening);
	Double f = 1 - GetSemiMinorAxis() / this->semiMajorAxis;
	this->eccentricity = Math::Sqrt(2 * f - f * f);
}

Math::EarthEllipsoid::EarthEllipsoid(EarthEllipsoidType eet)
{
	const Math::EarthEllipsoid::EarthEllipsoidInfo *info = GetEarthInfo(eet);
	this->eet = info->eet;
	this->semiMajorAxis = info->semiMajorAxis;
	this->inverseFlattening = info->inverseFlattening;
	this->semiMinorAxis = this->semiMajorAxis * (1.0 - 1.0 / this->inverseFlattening);
	Double f = 1 - GetSemiMinorAxis() / this->semiMajorAxis;
	this->eccentricity = Math::Sqrt(2 * f - f * f);
}

Math::EarthEllipsoid::EarthEllipsoid()
{
	const Math::EarthEllipsoid::EarthEllipsoidInfo *info = GetEarthInfo(Math::EarthEllipsoid::EET_OTHER);
	this->eet = info->eet;
	this->semiMajorAxis = info->semiMajorAxis;
	this->inverseFlattening = info->inverseFlattening;
	this->semiMinorAxis = this->semiMajorAxis * (1.0 - 1.0 / this->inverseFlattening);
	Double f = 1 - GetSemiMinorAxis() / this->semiMajorAxis;
	this->eccentricity = Math::Sqrt(2 * f - f * f);
}

Math::EarthEllipsoid::~EarthEllipsoid()
{
}

Double Math::EarthEllipsoid::CalSurfaceDistance(Double dLat1, Double dLon1, Double dLat2, Double dLon2, Math::Unit::Distance::DistanceUnit unit)
{
	Double r;
	Double rLat1;
	Double rLon1;
	Double rLat2;
	Double rLon2;
	if (dLat1 == dLat2 && dLon1 == dLon2)
		return 0;

	rLat1 = dLat1 * Math::PI / 180.0;
	rLon1 = dLon1 * Math::PI / 180.0;
	rLat2 = dLat2 * Math::PI / 180.0;
	rLon2 = dLon2 * Math::PI / 180.0;
	Double y = (rLat1 + rLat2) * 0.5;
	Double tmpV = this->eccentricity * Math::Sin(y);
	r = this->semiMajorAxis * (1 - this->eccentricity * this->eccentricity) / Math::Pow(1 - tmpV * tmpV, 1.5);
	Double cLat1 = Math::Cos(rLat1);
	Double cLat2 = Math::Cos(rLat2);
	Double d = Math::ArcCos(cLat1 * Math::Cos(rLon1) * cLat2 * Math::Cos(rLon2) + cLat1 * Math::Sin(rLon1) * cLat2 * Math::Sin(rLon2) + Math::Sin(rLat1) * Math::Sin(rLat2)) * r;
	if (d > 0 || d < 0)
	{
		if (unit != Math::Unit::Distance::DU_METER)
		{
			d = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_METER, unit, d);
		}
	}
	else if (d != 0)
	{
		d = 0;
	}
	return d;
}

Double Math::EarthEllipsoid::CalPLDistance(Math::Polyline *pl, Math::Unit::Distance::DistanceUnit unit)
{
	UOSInt nPoint;
	UOSInt nPtOfst;
	UInt32 *ptOfsts;
	Double *points;
	ptOfsts = pl->GetPtOfstList(&nPtOfst);
	points = pl->GetPointList(&nPoint);
	UOSInt i = nPtOfst;
	UOSInt j = nPoint;
	UOSInt k;
	Double totalDist = 0;
	Bool hasLast;
	Double lastX;
	Double lastY;
	while (i-- > 0)
	{
		k = ptOfsts[i];
		hasLast = false;
		while (j-- > k)
		{
			if (hasLast)
			{
				totalDist += CalSurfaceDistance(lastY, lastX, points[(j << 1) + 1], points[(j << 1)], unit);
			}
			hasLast = true;
			lastX = points[(j << 1)];
			lastY = points[(j << 1) + 1];
		}
		j++;
	}
	return totalDist;
}

Double Math::EarthEllipsoid::CalPLDistance3D(Math::Polyline3D *pl, Math::Unit::Distance::DistanceUnit unit)
{
	UOSInt nPoint;
	UOSInt nPtOfst;
	UOSInt nAlts;
	UInt32 *ptOfsts;
	Double *points;
	Double *alts;
	ptOfsts = pl->GetPtOfstList(&nPtOfst);
	points = pl->GetPointList(&nPoint);
	alts = pl->GetAltitudeList(&nAlts);
	UOSInt i = nPtOfst;
	UOSInt j = nPoint;
	UOSInt k;
	Double dist;
	Double totalDist = 0;
	Bool hasLast;
	Double lastX;
	Double lastY;
	Double lastH;
	while (i-- > 0)
	{
		k = ptOfsts[i];
		hasLast = false;
		while (j-- > k)
		{
			if (hasLast)
			{
				dist = CalSurfaceDistance(lastY, lastX, points[(j << 1) + 1], points[(j << 1)], unit);
				dist = Math::Sqrt(dist * dist + (alts[j] - lastH) * (alts[j] - lastH));
				totalDist += dist;
			}
			hasLast = true;
			lastX = points[(j << 1)];
			lastY = points[(j << 1) + 1];
			lastH = alts[j];
		}
		j++;
	}
	return totalDist;
}

Double Math::EarthEllipsoid::GetSemiMajorAxis()
{
	return this->semiMajorAxis;
}

Double Math::EarthEllipsoid::GetSemiMinorAxis()
{
	return this->semiMinorAxis;
}

Double Math::EarthEllipsoid::GetInverseFlattening()
{
	return this->inverseFlattening;
}

Double Math::EarthEllipsoid::GetEccentricity()
{
	return this->eccentricity;
}


// Vincenty's Formulae, Direct Method
Double Math::EarthEllipsoid::CalLonByDist(Double lat, Double lon, Double distM)
{
	Double rlat = lat * Math::PI / 180.0;
	Double r = CalRadiusAtLat(lat);
	Double diff = Math::ArcTan2(Math::Sin(distM / r) * Math::Cos(rlat), Math::Cos(distM / r));
	return lon + diff * 180.0 / Math::PI;
}

Double Math::EarthEllipsoid::CalLatByDist(Double lat, Double distM)
{
	Double r = CalRadiusAtLat(lat);
	Double rlat = lat * Math::PI / 180.0;
	return 180.0 / Math::PI * (rlat + (distM / r));
}

Double Math::EarthEllipsoid::CalRadiusAtLat(Double lat)
{
	Double rlat = lat * Math::PI / 180.0;
	Double ec = Math::Cos(rlat) * this->eccentricity;
	return this->semiMajorAxis / Math::Sqrt(1.0 - ec * ec);
}

Bool Math::EarthEllipsoid::Equals(Math::EarthEllipsoid *ellipsoid)
{
	return ellipsoid->semiMajorAxis == this->semiMajorAxis && ellipsoid->inverseFlattening == this->inverseFlattening;
}

const UTF8Char *Math::EarthEllipsoid::GetName()
{
	const EarthEllipsoidInfo *info = GetEarthInfo(this->eet);
	if (info == 0)
		return (const UTF8Char*)"Unknwon";
	return (const UTF8Char*)info->name;
}

void Math::EarthEllipsoid::operator=(const EarthEllipsoid &ellipsoid)
{
	this->semiMajorAxis = ellipsoid.semiMajorAxis;
	this->semiMinorAxis = ellipsoid.semiMinorAxis;
	this->inverseFlattening = ellipsoid.inverseFlattening;
	this->eccentricity = ellipsoid.eccentricity;
	this->eet = ellipsoid.eet;
}

void Math::EarthEllipsoid::operator=(const EarthEllipsoid *ellipsoid)
{
	this->semiMajorAxis = ellipsoid->semiMajorAxis;
	this->semiMinorAxis = ellipsoid->semiMinorAxis;
	this->inverseFlattening = ellipsoid->inverseFlattening;
	this->eccentricity = ellipsoid->eccentricity;
	this->eet = ellipsoid->eet;
}

Math::EarthEllipsoid *Math::EarthEllipsoid::Clone()
{
	Math::EarthEllipsoid *ellipsoid;
	NEW_CLASS(ellipsoid, Math::EarthEllipsoid(this->semiMajorAxis, this->inverseFlattening, this->eet));
	return ellipsoid;
}

void Math::EarthEllipsoid::ToCartesianCoord(Double dLat, Double dLon, Double h, Double *x, Double *y, Double *z)
{
	Double rLat = dLat * PI / 180.0;
	Double rLon = dLon * PI / 180.0;
	Double cLat = Math::Cos(rLat);
	Double sLat = Math::Sin(rLat);
	Double cLon = Math::Cos(rLon);
	Double sLon = Math::Sin(rLon);
	Double e2 = this->eccentricity * this->eccentricity;
	Double v = this->semiMajorAxis / Math::Sqrt(1 - e2 * sLat * sLat);
	*x = (v + h) * cLat * cLon;
	*y = (v + h) * cLat * sLon;
	*z = ((1 - e2) * v + h) * sLat;
}

void Math::EarthEllipsoid::FromCartesianCoord(Double x, Double y, Double z, Double *dLat, Double *dLon, Double *h)
{
	Double e2 = this->eccentricity * this->eccentricity;
	Double rLon = Math::ArcTan2(y, x);
	Double p = Math::Sqrt(x * x + y * y);
	Double rLat = Math::ArcTan2(z, p * (1 - e2));
	Double sLat;
	Double thisLat;
	Double v;
	OSInt i = 10;
	while (i-- > 0)
	{
		sLat = Math::Sin(rLat);
		v = this->semiMajorAxis / Math::Sqrt(1 - e2 * sLat * sLat);
		thisLat = Math::ArcTan2(z + e2 * v * sLat, p);
		if (thisLat == rLat)
			break;
		rLat = thisLat;
	}
	*dLat = rLat * 180 / PI;
	*dLon = rLon * 180 / PI;
	*h = p / Math::Cos(rLat) - v;
}

const Math::EarthEllipsoid::EarthEllipsoidInfo *Math::EarthEllipsoid::GetEarthInfo(EarthEllipsoidType eet)
{
	OSInt i = 0;
	OSInt j = (sizeof(refEllipsoids) / sizeof(refEllipsoids[0])) - 1;
	OSInt k;
	while (i <= j)
	{
		k = (i + j) >> 1;
		if (refEllipsoids[k].eet > eet)
		{
			j = k - 1;
		}
		else if (refEllipsoids[k].eet < eet)
		{
			i = k + 1;
		}
		else
		{
			return &refEllipsoids[k];
		}
	}
	return &refEllipsoids[0];
}
