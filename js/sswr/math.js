import * as unit from "./unit.js";

export function roundToFloat(n, decimalPoints)
{
	if (decimalPoints === undefined) {
		decimalPoints = 0;
	}

	var multiplicator = Math.pow(10, decimalPoints);
	n = parseFloat((n * multiplicator).toFixed(11));
	return Math.round(n) / multiplicator;
}

export function roundToStr(n, decimalPoints)
{
	if (decimalPoints === undefined) {
		decimalPoints = 0;
	}

	var multiplicator = Math.pow(10, decimalPoints);
	n = parseFloat((n * multiplicator).toFixed(11));
	var s = "" + Math.round(n);
	if (decimalPoints == 0)
	{
		return s;
	}
	else if (s.length > decimalPoints)
	{
		return s.substring(0, s.length - decimalPoints) + "." + s.substring(s.length - decimalPoints);
	}
	else
	{
		return "0."+("0".repeat(decimalPoints - s.length)) + s;
	}
}

export class GeoJSON {
	static parseGeometry(srid, geom)
	{
		if (geom.type == "Polygon")
		{
			return new geometry.Polygon(srid, geom.coordinates);
		}
		else if (geom.type == "MultiPolygon")
		{
			return new geometry.MultiPolygon(srid, geom.coordinates);
		}
		else
		{
			console.log("GeoJSON.parseGeometry: unknown type "+geom.type);
		}
		return null;
	}
}

export class Coord2D
{
	constructor(x, y)
	{
		this.x = x;
		this.y = y;
	}

	get lat()
	{
		return this.y;
	}
	
	get lon()
	{
		return this.x;
	}
}

export class Vector3 extends Coord2D
{
	constructor(x, y, z)
	{
		super(x, y);
		this.z = z;
	}

	get height()
	{
		return this.z;
	}
}

export const EarthEllipsoidType = {
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

export class EarthEllipsoid
{
	constructor(semiMajorAxis, inverseFlattening, eet)
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

	calSurfaceDistance(dLat1, dLon1, dLat2, dLon2, distUnit)
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
			if (distUnit != null && distUnit != unit.DistanceUnit.METER)
			{
				d = unit.Distance.convert(unit.DistanceUnit.METER, distUnit, d);
			}
		}
		else if (d != 0)
		{
			d = 0;
		}
		return d;	
	}

	getSemiMinorAxis()
	{
		return this.semiMinorAxis;
	}

	initEarthInfo(eet)
	{
		switch (eet)
		{
		default:
		case EarthEllipsoidType.OTHER:
			this.semiMajorAxis = 6378137.0;
			this.inverseFlattening = 191.0;
			return;
		case EarthEllipsoidType.PLESSIS:
			this.semiMajorAxis = 6376523.0;
			this.inverseFlattening = 308.64;
			return;
		case EarthEllipsoidType.EVEREST1830:
			this.semiMajorAxis = 6377299.365;
			this.inverseFlattening = 300.80172554;
			return;
		case EarthEllipsoidType.EVEREST1830M:
			this.semiMajorAxis = 6377304.063;
			this.inverseFlattening = 300.8017;
			return;
		case EarthEllipsoidType.EVEREST1830N:
			this.semiMajorAxis = 6377298.556;
			this.inverseFlattening = 300.8017;
			return;
		case EarthEllipsoidType.AIRY1830:
			this.semiMajorAxis = 6377563.396;
			this.inverseFlattening = 299.3249646;
			return;
		case EarthEllipsoidType.AIRY1830M:
			this.semiMajorAxis = 6377340.189;
			this.inverseFlattening = 299.32495141450600500090538973015;
			return;
		case EarthEllipsoidType.BESSEL1841:
			this.semiMajorAxis = 6377397.155;
			this.inverseFlattening = 299.1528128;
			return;
		case EarthEllipsoidType.CLARKE1866:
			this.semiMajorAxis = 6378206.4;
			this.inverseFlattening = 294.9786982;
			return;
		case EarthEllipsoidType.CLARKE1878:
			this.semiMajorAxis = 6378190.0;
			this.inverseFlattening = 293.4659980;
			return;
		case EarthEllipsoidType.CLARKE1880:
			this.semiMajorAxis = 6378249.145;
			this.inverseFlattening = 293.465;
			return;
		case EarthEllipsoidType.HELMERT1906:
			this.semiMajorAxis = 6378200.0;
			this.inverseFlattening = 298.3;
			return;
		case EarthEllipsoidType.HAYFORD1910:
			this.semiMajorAxis = 6378388.0;
			this.inverseFlattening = 297.0;
			return;
		case EarthEllipsoidType.INTL1924:
			this.semiMajorAxis = 6378388.0;
			this.inverseFlattening = 297.0;
			return;
		case EarthEllipsoidType.KRASSOVSKY1940:
			this.semiMajorAxis = 6378245.0;
			this.inverseFlattening = 298.3;
			return;
		case EarthEllipsoidType.WGS66:
			this.semiMajorAxis = 6378145.0;
			this.inverseFlattening = 298.25;
			return;
		case EarthEllipsoidType.AUSTRALIAN1966:
			this.semiMajorAxis = 6378160.0;
			this.inverseFlattening = 298.25;
			return;
		case EarthEllipsoidType.NEWINTL1967:
			this.semiMajorAxis = 6378157.5;
			this.inverseFlattening = 298.24961539;
			return;
		case EarthEllipsoidType.GPS67:
			this.semiMajorAxis = 6378160.0;
			this.inverseFlattening = 298.247167427;
			return;
		case EarthEllipsoidType.SAM1969:
			this.semiMajorAxis = 6378160.0;
			this.inverseFlattening = 298.25;
			return;
		case EarthEllipsoidType.WGS72:
			this.semiMajorAxis = 6378135.0;
			this.inverseFlattening = 298.26;
			return;
		case EarthEllipsoidType.GRS80:
			this.semiMajorAxis = 6378137.0;
			this.inverseFlattening = 298.257222101;
			return;
		case EarthEllipsoidType.WGS84:
			this.semiMajorAxis = 6378137.0;
			this.inverseFlattening = 298.257223563;
			return;
		case EarthEllipsoidType.WGS84_OGC:
			this.semiMajorAxis = 6378137.0;
			this.inverseFlattening = 298.257222932867;
			return;
		case EarthEllipsoidType.IERS1989:
			this.semiMajorAxis = 6378136.0;
			this.inverseFlattening = 298.257;
			return;
		case EarthEllipsoidType.IERS2003:
			this.semiMajorAxis = 6378136.6;
			this.inverseFlattening = 298.25642;
			return;
		}
	}
}

export class Spheroid
{
	constructor(srid, ellipsoid, name)
	{
		this.srid = srid;
		this.ellipsoid = ellipsoid;
		this.name = name;
	};
}

export class DatumData
{
	constructor(srid, spheroid, name, x0, y0, z0, cX, cY, cZ, xAngle, yAngle, zAngle, scale, aunit)
	{
		this.srid = srid;
		this.spheroid = spheroid;
		this.name = name;
		this.x0 = x0;
		this.y0 = y0;
		this.z0 = z0;
		this.cX = cX;
		this.cY = cY;
		this.cZ = cZ;
		var aratio = unit.Angle.getUnitRatio(aunit);
		this.xAngle = xAngle * aratio;
		this.yAngle = yAngle * aratio;
		this.zAngle = zAngle * aratio;
		this.scale = scale;
		this.aunit = unit.AngleUnit.RADIAN;
	}
}

export const CoordinateSystemType = {
	Geographic: 0,
	MercatorProjected: 1,
	Mercator1SPProjected: 2,
	PointMapping: 3,
	GausskrugerProjected: 4
};

export class CoordinateSystem
{
	constructor(srid, csysName)
	{
		this.srid = srid;
		this.csysName = csysName;
	};
}

export class GeographicCoordinateSystem extends CoordinateSystem
{
	constructor(srid, csysName, datumData)
	{
		super(srid, csysName);
		this.datum = datumData;
	}

	calcSurfaceDistance(x1, y1, x2, y2, unit)
	{
		return this.datum.spheroid.ellipsoid.calSurfaceDistance(y1, x1, y2, x2, unit);
	}
}

export class ProjectedCoordinateSystem extends CoordinateSystem
{
	constructor(srid, csysName, falseEasting, falseNorthing, dcentralMeridian, dlatitudeOfOrigin, scaleFactor, gcs)
	{
		super(srid, csysName);
		this.falseEasting = falseEasting;
		this.falseNorthing = falseNorthing;
		this.rcentralMeridian = dcentralMeridian * Math.PI / 180;
		this.rlatitudeOfOrigin = dlatitudeOfOrigin * Math.PI / 180;
		this.scaleFactor = scaleFactor;
		this.gcs = gcs;
	}

	calcSurfaceDistance(x1, y1, x2, y2, distUnit)
	{
		var diffX = x2 - x1;
		var diffY = y2 - y1;
		diffX = diffX * diffX;
		diffY = diffY * diffY;
		var d = Math.sqrt(diffX + diffY);
		if (distUnit != unit.DistanceUnit.METER)
		{
			d = unit.Distance.convert(unit.DistanceUnit.METER, distUnit, d);
		}
		return d;
	}
}

export class MercatorProjectedCoordinateSystem extends ProjectedCoordinateSystem
{
	constructor(srid, csysName, falseEasting, falseNorthing, dcentralMeridian, dlatitudeOfOrigin, scaleFactor, gcs)
	{
		super(srid, csysName, falseEasting, falseNorthing, dcentralMeridian, dlatitudeOfOrigin, scaleFactor, gcs);
	}
}

export class Mercator1SPProjectedCoordinateSystem extends ProjectedCoordinateSystem
{
	constructor(srid, csysName, falseEasting, falseNorthing, dcentralMeridian, dlatitudeOfOrigin, scaleFactor, gcs)
	{
		super(srid, csysName, falseEasting, falseNorthing, dcentralMeridian, dlatitudeOfOrigin, scaleFactor, gcs);
	}
}

export class CoordinateSystemManager
{
	static srCreateGeogCSys(srid, datumSrid, name)
	{
		var data = this.srGetDatumData(datumSrid);
		if (data == null)
		{
			return null;
		}
		return new GeographicCoordinateSystem(srid, name, data);
	}
	
	static srCreateProjCSys(srid, geogcsSrid, csysType, projName, falseEasting, falseNorthing, centralMeridian, latitudeOfOrigin, scaleFactor)
	{
		var gcsys = this.srCreateGeogCSys(geogcsSrid);
		if (gcsys == null)
			return null;
		if (csysType == CoordinateSystemType.MercatorProjected || csysType == CoordinateSystemType.GausskrugerProjected)
		{
			return new MercatorProjectedCoordinateSystem(srid, projName, falseEasting, falseNorthing, centralMeridian, latitudeOfOrigin, scaleFactor, gcsys);
		}
		else if (csysType == CoordinateSystemType.Mercator1SPProjected)
		{
			return new Mercator1SPProjectedCoordinateSystem(srid, projName, falseEasting, falseNorthing, centralMeridian, latitudeOfOrigin, scaleFactor, gcsys);
		}
		return null;
	}
	
	static srCreateCsys(srid)
	{
		switch (srid)
		{
		case 2326:
		case 102140:
			return this.srCreateProjCSys(srid, 4611, CoordinateSystemType.MercatorProjected, "Hong Kong 1980 Grid System", 836694.05, 819069.80, 114.17855555555555555555555555556, 22.312133333333333333333333333333, 1);
		case 3857:
		case 900913:
			return this.srCreateProjCSys(srid, 4326, CoordinateSystemType.Mercator1SPProjected, "WGS 84 / Pseudo-Mercator", 0, 0, 0, 0, 1);
		case 4326:
			return this.srCreateGeogCSys(srid, 6326, "WGS 84");
		case 4611:
			return this.srCreateGeogCSys(srid, 6611, "Hong Kong 1980");
		default:
			return null;
		}
	}
	
	static srGetDatumData(srid)
	{
		switch (srid)
		{
		case 6326:
			return new DatumData(6326, this.srGetSpheroid(7030), "WGS_1984", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, unit.AngleUnit.RADIAN);
		case 6600:
			return new DatumData(6600, this.srGetSpheroid(7012), "Anguilla_1957", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, unit.AngleUnit.RADIAN);
		case 6601:
			return new DatumData(6601, this.srGetSpheroid(7012), "Antigua_1943", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, unit.AngleUnit.RADIAN);
		case 6602:
			return new DatumData(6602, this.srGetSpheroid(7012), "Dominica_1945", 0, 0, 0, 725, 685, 536, 0, 0, 0, 0, unit.AngleUnit.ARCSECOND);
		case 6603:
			return new DatumData(6603, this.srGetSpheroid(7012), "Grenada_1953", 0, 0, 0, 72, 213.7, 93, 0, 0, 0, 0, unit.AngleUnit.ARCSECOND);
		case 6611:
			return new DatumData(6611, this.srGetSpheroid(7022), "Hong_Kong_1980", 0, 0, 0, -162.619, -276.959, -161.764, 0.067753, -2.24365, -1.15883, -1.09425, unit.AngleUnit.ARCSECOND);
		}
		return null;
	}
	
	static srGetSpheroid(srid)
	{
		switch (srid)
		{
		case 7012:
			return new Spheroid(7012, new EarthEllipsoid(null, null, EarthEllipsoidType.CLARKE1880), "Clarke 1880 (RGS)");
		case 7022:
			return new Spheroid(7022, new EarthEllipsoid(null, null, EarthEllipsoidType.INTL1924),   "International 1924");
		case 7030:
			return new Spheroid(7030, new EarthEllipsoid(null, null, EarthEllipsoidType.WGS84),      "WGS 84");
		}
		return null;
	}	
};
