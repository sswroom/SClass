#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Math/EarthEllipsoid.h"

Math::EarthEllipsoid::EarthEllipsoidInfo Math::EarthEllipsoid::refEllipsoids[] = {
	{EET_OTHER,			CSTR("Other"),						1984, 6378137.0,		191.0},
	{EET_PLESSIS,		CSTR("Plessis"),					1817, 6376523.0,		308.64},
	{EET_EVEREST1830,	CSTR("Everest"),					1830, 6377299.365,		300.80172554},
	{EET_EVEREST1830A,	CSTR("Everest 1830 (1937 Adjustment)"),	1937, 6377276.345,		300.8017},
	{EET_EVEREST1830M,	CSTR("Everest 1830 Modified"),		1967, 6377304.063,		300.8017},
	{EET_EVEREST1830N,	CSTR("Everest 1830 (1967 def)"),	1967, 6377298.556,		300.8017},
	{EET_AIRY1830,		CSTR("Airy"),						1830, 6377563.396,		299.3249646},
	{EET_AIRY1830M,		CSTR("Airy (Modified)"),			1830, 6377340.189,		299.32495141450600500090538973015},
	{EET_AIRYM1849,		CSTR("Airy Modified 1849"),			1849, 6377340.189,		299.3249646},
	{EET_BESSEL1841,	CSTR("Bessel"),						1841, 6377397.155,		299.1528128},
	{EET_BESSELMOD,		CSTR("Bessel Modified"),			1841, 6377492.018,		299.1528128},
	{EET_BESSELNAMIBIA,	CSTR("Bessel Namibia"),				1841, 6377483.865,		299.1528128},
	{EET_CLARKE1858,	CSTR("Clarke 1858"),				1858, 6378293.64520876,	294.260676369261},
	{EET_CLARKE1866,	CSTR("Clarke 1866"),				1866, 6378206.4,		294.978698213898},
	{EET_CLARKE1866M,	CSTR("Clarke 1866 Michigan"),		1866, 6378450.0475489,	294.978697164677},
	{EET_CLARKE1878,	CSTR("Clarke 1878"),				1878, 6378190.0,		293.4659980},
	{EET_CLARKE1880A,	CSTR("Clarke 1880 (Arc)"),			1880, 6378249.145,		293.4663077},
	{EET_CLARKE1880B,	CSTR("Clarke 1880 (Benoit)"),		1880, 6378300.789,		293.466315538981},
	{EET_CLARKE1880I,	CSTR("Clarke 1880 (IGN)"),			1880, 6378249.2,		293.466021293627},
	{EET_CLARKE1880R,	CSTR("Clarke 1880 (RGS)"),			1880, 6378249.145,		293.465},
	{EET_CLARKE1880S,	CSTR("Clarke 1880 (SGA 1922)"),		1880, 6378249.2,		293.46598},
	{EET_HELMERT1906,	CSTR("Helmert"),					1906, 6378200.0,		298.3},
	{EET_HAYFORD1910,	CSTR("Hayford"),					1910, 6378388.0,		297.0},
	{EET_INTL1924,		CSTR("International"),				1924, 6378388.0,		297.0},
	{EET_KRASSOVSKY1940, CSTR("Krassovsky"),				1940, 6378245.0,		298.3},
	{EET_WGS66,			CSTR("WGS66"),						1966, 6378145.0,		298.25},
	{EET_AUSTRALIAN1966, CSTR("Australian National"),		1966, 6378160.0,		298.25},
	{EET_NEWINTL1967,	CSTR("New International"),			1967, 6378157.5,		298.24961539},
	{EET_GPS67,			CSTR("GPS-67"),						1967, 6378160.0,		298.247167427},
	{EET_SAM1969,		CSTR("South American"),				1969, 6378160.0,		298.25},
	{EET_WGS72,			CSTR("WGS-72"),						1972, 6378135.0,		298.26},
	{EET_GRS80,			CSTR("GRS-80"),						1979, 6378137.0,		298.257222101},
	{EET_WGS84,			CSTR("WGS-84"),						1984, 6378137.0,		298.257223563},
	{EET_WGS84_OGC,		CSTR("WGS-84 (OGC Values)"),		1984, 6378137.0,		298.257222932867},
	{EET_IERS1989,		CSTR("IERS"),						1989, 6378136.0,		298.257},
	{EET_IERS2003,		CSTR("IERS"),						2003, 6378136.6,		298.25642}
};

Math::EarthEllipsoid::EarthEllipsoid(Double semiMajorAxis, Double inverseFlattening, EarthEllipsoidType eet)
{
	this->eet = eet;
	this->semiMajorAxis = semiMajorAxis;
	this->inverseFlattening = inverseFlattening;
	this->semiMinorAxis = this->semiMajorAxis * (1.0 - 1.0 / this->inverseFlattening);
	Double f = 1 - GetSemiMinorAxis() / this->semiMajorAxis;
	this->eccentricity = Math_Sqrt(2 * f - f * f);
}

Math::EarthEllipsoid::EarthEllipsoid(EarthEllipsoidType eet)
{
	NotNullPtr<const Math::EarthEllipsoid::EarthEllipsoidInfo> info = GetEarthInfo(eet);
	this->eet = info->eet;
	this->semiMajorAxis = info->semiMajorAxis;
	this->inverseFlattening = info->inverseFlattening;
	this->semiMinorAxis = this->semiMajorAxis * (1.0 - 1.0 / this->inverseFlattening);
	Double f = 1 - GetSemiMinorAxis() / this->semiMajorAxis;
	this->eccentricity = Math_Sqrt(2 * f - f * f);
}

Math::EarthEllipsoid::EarthEllipsoid()
{
	NotNullPtr<const Math::EarthEllipsoid::EarthEllipsoidInfo> info = GetEarthInfo(Math::EarthEllipsoid::EET_OTHER);
	this->eet = info->eet;
	this->semiMajorAxis = info->semiMajorAxis;
	this->inverseFlattening = info->inverseFlattening;
	this->semiMinorAxis = this->semiMajorAxis * (1.0 - 1.0 / this->inverseFlattening);
	Double f = 1 - GetSemiMinorAxis() / this->semiMajorAxis;
	this->eccentricity = Math_Sqrt(2 * f - f * f);
}

Math::EarthEllipsoid::~EarthEllipsoid()
{
}

Double Math::EarthEllipsoid::CalSurfaceDistance(Double dLat1, Double dLon1, Double dLat2, Double dLon2, Math::Unit::Distance::DistanceUnit unit) const
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
	Double tmpV = this->eccentricity * Math_Sin(y);
	r = this->semiMajorAxis * (1 - this->eccentricity * this->eccentricity) / Math_Pow(1 - tmpV * tmpV, 1.5);
	Double cLat1 = Math_Cos(rLat1);
	Double cLat2 = Math_Cos(rLat2);
	Double d = Math_ArcCos(cLat1 * Math_Cos(rLon1) * cLat2 * Math_Cos(rLon2) + cLat1 * Math_Sin(rLon1) * cLat2 * Math_Sin(rLon2) + Math_Sin(rLat1) * Math_Sin(rLat2)) * r;
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

Double Math::EarthEllipsoid::CalPLDistance(NotNullPtr<Math::Geometry::Polyline> pl, Math::Unit::Distance::DistanceUnit unit) const
{
	UOSInt nPoint;
	UOSInt nPtOfst;
	UInt32 *ptOfsts;
	Math::Coord2DDbl *points;
	ptOfsts = pl->GetPtOfstList(nPtOfst);
	points = pl->GetPointList(nPoint);
	UOSInt i = nPtOfst;
	UOSInt j = nPoint;
	UOSInt k;
	Double totalDist = 0;
	Math::Coord2DDbl lastPt;
	while (i-- > 0)
	{
		k = ptOfsts[i];
		if (j-- > k)
		{
			lastPt = points[j];
			while (j-- > k)
			{
				totalDist += CalSurfaceDistance(lastPt.y, lastPt.x, points[j].y, points[j].x, unit);
				lastPt = points[j];
			}
		}
		j++;
	}
	return totalDist;
}

Double Math::EarthEllipsoid::CalPLDistance3D(NotNullPtr<Math::Geometry::Polyline> pl, Math::Unit::Distance::DistanceUnit unit) const
{
	UOSInt nPoint;
	UOSInt nPtOfst;
	UOSInt nAlts;
	UInt32 *ptOfsts;
	Math::Coord2DDbl *points;
	Double *alts;
	ptOfsts = pl->GetPtOfstList(nPtOfst);
	points = pl->GetPointList(nPoint);
	alts = pl->GetZList(&nAlts);
	UOSInt i = nPtOfst;
	UOSInt j = nPoint;
	UOSInt k;
	Double dist;
	Double totalDist = 0;
	Math::Coord2DDbl lastPt;
	Double lastH;
	Double altDiff;
	while (i-- > 0)
	{
		k = ptOfsts[i];
		if (j-- > k)
		{
			lastPt = points[j];
			lastH = alts[j];
			while (j-- > k)
			{
				dist = CalSurfaceDistance(lastPt.y, lastPt.x, points[j].y, points[j].x, unit);
				altDiff = (alts[j] - lastH);
				dist = Math_Sqrt(dist * dist + altDiff * altDiff);
				totalDist += dist;
				lastPt = points[j];
				lastH = alts[j];
			}
		}
		j++;
	}
	return totalDist;
}

// Vincenty's Formulae, Direct Method
Double Math::EarthEllipsoid::CalLonByDist(Double lat, Double lon, Double distM) const
{
	Double rlat = lat * Math::PI / 180.0;
	Double r = CalRadiusAtRLat(rlat);
	Double diff = Math_ArcTan2(Math_Sin(distM / r) * Math_Cos(rlat), Math_Cos(distM / r));
	return lon + diff * 180.0 / Math::PI;
}

Double Math::EarthEllipsoid::CalLatByDist(Double lat, Double distM) const
{
	Double rlat = lat * Math::PI / 180.0;
	Double r = CalRadiusAtRLat(rlat);
	Double rlat2 = rlat + (distM / r);
	Double r2 = CalRadiusAtRLat(rlat2);
	return 180.0 / Math::PI * (rlat + (distM / ((r + r2) * 0.5)));
}

Text::CStringNN Math::EarthEllipsoid::GetName() const
{
	return GetEarthInfo(this->eet)->name;
}

void Math::EarthEllipsoid::ToString(NotNullPtr<Text::StringBuilderUTF8> sb) const
{
	sb->AppendC(UTF8STRC("Semi-Major Axis: "));
	Text::SBAppendF64(sb, this->GetSemiMajorAxis());
	sb->AppendC(UTF8STRC("\r\nInverse Flattening: "));
	Text::SBAppendF64(sb, this->GetInverseFlattening());
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

NotNullPtr<Math::EarthEllipsoid> Math::EarthEllipsoid::Clone() const
{
	NotNullPtr<Math::EarthEllipsoid> ellipsoid;
	NEW_CLASSNN(ellipsoid, Math::EarthEllipsoid(this->semiMajorAxis, this->inverseFlattening, this->eet));
	return ellipsoid;
}

Math::Vector3 Math::EarthEllipsoid::ToCartesianCoordRad(Math::Vector3 lonLatH) const
{
	Double cLat = Math_Cos(lonLatH.GetLat());
	Double sLat = Math_Sin(lonLatH.GetLat());
	Double cLon = Math_Cos(lonLatH.GetLon());
	Double sLon = Math_Sin(lonLatH.GetLon());
	Double e2 = this->eccentricity * this->eccentricity;
	Double v = this->semiMajorAxis / Math_Sqrt(1 - e2 * sLat * sLat);
	return Math::Vector3(
		(v + lonLatH.GetH()) * cLat * cLon,
		(v + lonLatH.GetH()) * cLat * sLon,
		((1 - e2) * v + lonLatH.GetH()) * sLat);
}

Math::Vector3 Math::EarthEllipsoid::FromCartesianCoordRad(Math::Vector3 coord) const
{
	Double e2 = this->eccentricity * this->eccentricity;
	Double rLon = Math_ArcTan2(coord.GetY(), coord.GetX());
	Double p = Math_Sqrt(coord.GetX() * coord.GetX() + coord.GetY() * coord.GetY());
	Double rLat = Math_ArcTan2(coord.GetZ(), p * (1 - e2));
	Double sLat;
	Double thisLat;
	Double v = 0;
	OSInt i = 10;
	while (i-- > 0)
	{
		sLat = Math_Sin(rLat);
		v = this->semiMajorAxis / Math_Sqrt(1 - e2 * sLat * sLat);
		thisLat = Math_ArcTan2(coord.GetZ() + e2 * v * sLat, p);
		if (thisLat == rLat)
			break;
		rLat = thisLat;
	}
	return Math::Vector3(rLon, rLat, p / Math_Cos(rLat) - v);
}

NotNullPtr<const Math::EarthEllipsoid::EarthEllipsoidInfo> Math::EarthEllipsoid::GetEarthInfo(EarthEllipsoidType eet)
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
			return refEllipsoids[k];
		}
	}
	return refEllipsoids[0];
}
