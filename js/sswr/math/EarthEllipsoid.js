import unit from "./unit";
var EarthEllipsoid = function(semiMajorAxis, inverseFlattening, eet)
{
	this.eet = eet;
	if (semiMajorAxis == null || inverseFlattening == null)
	{
		this.initEarthInfo(eet);
	}
	else
	{
		this.semiMajorAxis = semiMajorAxis;
		this.inverseFlattening = inverseFlattening;
	}
	this.semiMinorAxis = this.semiMajorAxis * (1.0 - 1.0 / this.inverseFlattening);
	var f = 1 - this.getSemiMinorAxis() / this.semiMajorAxis;
	this.eccentricity = Math.sqrt(2 * f - f * f);
};

EarthEllipsoid.EarthEllipsoidType = {
	OTHER: 0,
	PLESSIS: 1,
	EVEREST1830: 2,
	EVEREST1830M: 3,
	EVEREST1830N: 4,
	AIRY1830: 5,
	AIRY1830M: 6,
	BESSEL1841: 7,
	CLARKE1866: 8,
	CLARKE1878: 9,
	CLARKE1880: 10,
	HELMERT1906: 11,
	HAYFORD1910: 12,
	INTL1924: 13,
	KRASSOVSKY1940: 14,
	WGS66: 15,
	AUSTRALIAN1966: 16,
	NEWINTL1967: 17,
	GPS67: 18,
	SAM1969: 19,
	WGS72: 20,
	GRS80: 21,
	WGS84: 22,
	WGS84_OGC: 23,
	IERS1989: 24,
	IERS2003: 25
};

EarthEllipsoid.prototype.calSurfaceDistance = function(dLat1, dLon1, dLat2, dLon2, distUnit)
{
	var r;
	var rLat1;
	var rLon1;
	var rLat2;
	var rLon2;
	if (dLat1 == dLat2 && dLon1 == dLon2)
		return 0;

	rLat1 = dLat1 * Math.PI / 180.0;
	rLon1 = dLon1 * Math.PI / 180.0;
	rLat2 = dLat2 * Math.PI / 180.0;
	rLon2 = dLon2 * Math.PI / 180.0;
	var y = (rLat1 + rLat2) * 0.5;
	var tmpV = this.eccentricity * Math.sin(y);
	r = this.semiMajorAxis * (1 - this.eccentricity * this.eccentricity) / Math.pow(1 - tmpV * tmpV, 1.5);
	var cLat1 = Math.cos(rLat1);
	var cLat2 = Math.cos(rLat2);
	var d = Math.acos(cLat1 * Math.cos(rLon1) * cLat2 * Math.cos(rLon2) + cLat1 * Math.sin(rLon1) * cLat2 * Math.sin(rLon2) + Math.sin(rLat1) * Math.sin(rLat2)) * r;
	if (d > 0 || d < 0)
	{
		if (distUnit != null && distUnit != unit.Distance.DistanceUnit.METER)
		{
			d = unit.Distance.convert(unit.Distance.DistanceUnit.METER, distUnit, d);
		}
	}
	else if (d != 0)
	{
		d = 0;
	}
	return d;	
}

EarthEllipsoid.prototype.getSemiMinorAxis = function()
{
	return this.semiMinorAxis;
}

EarthEllipsoid.prototype.initEarthInfo = function(eet)
{
	switch (eet)
	{
	default:
	case EarthEllipsoid.EarthEllipsoidType.OTHER:
		this.semiMajorAxis = 6378137.0;
		this.inverseFlattening = 191.0;
		return;
	case EarthEllipsoid.EarthEllipsoidType.PLESSIS:
		this.semiMajorAxis = 6376523.0;
		this.inverseFlattening = 308.64;
		return;
	case EarthEllipsoid.EarthEllipsoidType.EVEREST1830:
		this.semiMajorAxis = 6377299.365;
		this.inverseFlattening = 300.80172554;
		return;
	case EarthEllipsoid.EarthEllipsoidType.EVEREST1830M:
		this.semiMajorAxis = 6377304.063;
		this.inverseFlattening = 300.8017;
		return;
	case EarthEllipsoid.EarthEllipsoidType.EVEREST1830N:
		this.semiMajorAxis = 6377298.556;
		this.inverseFlattening = 300.8017;
		return;
	case EarthEllipsoid.EarthEllipsoidType.AIRY1830:
		this.semiMajorAxis = 6377563.396;
		this.inverseFlattening = 299.3249646;
		return;
	case EarthEllipsoid.EarthEllipsoidType.AIRY1830M:
		this.semiMajorAxis = 6377340.189;
		this.inverseFlattening = 299.32495141450600500090538973015;
		return;
	case EarthEllipsoid.EarthEllipsoidType.BESSEL1841:
		this.semiMajorAxis = 6377397.155;
		this.inverseFlattening = 299.1528128;
		return;
	case EarthEllipsoid.EarthEllipsoidType.CLARKE1866:
		this.semiMajorAxis = 6378206.4;
		this.inverseFlattening = 294.9786982;
		return;
	case EarthEllipsoid.EarthEllipsoidType.CLARKE1878:
		this.semiMajorAxis = 6378190.0;
		this.inverseFlattening = 293.4659980;
		return;
	case EarthEllipsoid.EarthEllipsoidType.CLARKE1880:
		this.semiMajorAxis = 6378249.145;
		this.inverseFlattening = 293.465;
		return;
	case EarthEllipsoid.EarthEllipsoidType.HELMERT1906:
		this.semiMajorAxis = 6378200.0;
		this.inverseFlattening = 298.3;
		return;
	case EarthEllipsoid.EarthEllipsoidType.HAYFORD1910:
		this.semiMajorAxis = 6378388.0;
		this.inverseFlattening = 297.0;
		return;
	case EarthEllipsoid.EarthEllipsoidType.INTL1924:
		this.semiMajorAxis = 6378388.0;
		this.inverseFlattening = 297.0;
		return;
	case EarthEllipsoid.EarthEllipsoidType.KRASSOVSKY1940:
		this.semiMajorAxis = 6378245.0;
		this.inverseFlattening = 298.3;
		return;
	case EarthEllipsoid.EarthEllipsoidType.WGS66:
		this.semiMajorAxis = 6378145.0;
		this.inverseFlattening = 298.25;
		return;
	case EarthEllipsoid.EarthEllipsoidType.AUSTRALIAN1966:
		this.semiMajorAxis = 6378160.0;
		this.inverseFlattening = 298.25;
		return;
	case EarthEllipsoid.EarthEllipsoidType.NEWINTL1967:
		this.semiMajorAxis = 6378157.5;
		this.inverseFlattening = 298.24961539;
		return;
	case EarthEllipsoid.EarthEllipsoidType.GPS67:
		this.semiMajorAxis = 6378160.0;
		this.inverseFlattening = 298.247167427;
		return;
	case EarthEllipsoid.EarthEllipsoidType.SAM1969:
		this.semiMajorAxis = 6378160.0;
		this.inverseFlattening = 298.25;
		return;
	case EarthEllipsoid.EarthEllipsoidType.WGS72:
		this.semiMajorAxis = 6378135.0;
		this.inverseFlattening = 298.26;
		return;
	case EarthEllipsoid.EarthEllipsoidType.GRS80:
		this.semiMajorAxis = 6378137.0;
		this.inverseFlattening = 298.257222101;
		return;
	case EarthEllipsoid.EarthEllipsoidType.WGS84:
		this.semiMajorAxis = 6378137.0;
		this.inverseFlattening = 298.257223563;
		return;
	case EarthEllipsoid.EarthEllipsoidType.WGS84_OGC:
		this.semiMajorAxis = 6378137.0;
		this.inverseFlattening = 298.257222932867;
		return;
	case EarthEllipsoid.EarthEllipsoidType.IERS1989:
		this.semiMajorAxis = 6378136.0;
		this.inverseFlattening = 298.257;
		return;
	case EarthEllipsoid.EarthEllipsoidType.IERS2003:
		this.semiMajorAxis = 6378136.6;
		this.inverseFlattening = 298.25642;
		return;
	}
}
export default EarthEllipsoid;