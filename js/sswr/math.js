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

	mul(val)
	{
		return new Coord2D(this.x * val, this.y * val);
	}
}

export class RectArea
{
	constructor(x1, y1, x2, y2)
	{
		var minX;
		var maxX;
		if (x1 > x2)
		{
			maxX = x1;
			minX = x2;
		}
		else
		{
			maxX = x2;
			minX = x1;
		}
		if (y1 > y2)
		{
			this.min = new Coord2D(minX, y2);
			this.max = new Coord2D(maxX, y1);
		}
		else
		{
			this.min = new Coord2D(minX, y1);
			this.max = new Coord2D(maxX, y2);
		}
	}

	get minX()
	{
		return this.min.x;
	}

	get minY()
	{
		return this.min.y;
	}

	get maxX()
	{
		return this.max.x;
	}

	get maxY()
	{
		return this.max.y;
	}

	containPt(x, y)
	{
		return (x >= this.min.x && x <= this.max.x && y >= this.min.y && y <= this.max.y);
	}

	getCenter()
	{
		return new Coord2D((this.min.x + this.max.x) * 0.5, (this.min.y + this.max.y) * 0.5);
	}

	getWidth()
	{
		return this.max.x - this.min.x;
	}

	getHeight()
	{
		return this.max.y - this.min.y;
	}

	getArea()
	{
		return this.getWidth() * this.getHeight();
	}
};

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

	mulXY(val)
	{
		return new Vector3(this.x * val, this.y * val, this.z);
	}

	mul(val)
	{
		return new Vector3(this.x * val, this.y * val, this.z * val);
	}

	static fromCoord2D(coord, z)
	{
		return new Vector3(coord.x, coord.y, z);
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
			if (distUnit != null && distUnit != unit.Distance.Unit.METER)
			{
				d = unit.Distance.convert(unit.Distance.Unit.METER, distUnit, d);
			}
		}
		else if (d != 0)
		{
			d = 0;
		}
		return d;	
	}

	getSemiMajorAxis()
	{
		return this.semiMajorAxis;
	}
	getSemiMinorAxis()
	{
		return this.semiMinorAxis;
	}

	getInverseFlattening()
	{
		return this.inverseFlattening;
	}

	getEccentricity()
	{
		return this.eccentricity;
	}

	equals(ellipsoid)
	{
		return ellipsoid.semiMajorAxis == this.semiMajorAxis && ellipsoid.inverseFlattening == this.inverseFlattening;
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
	toCartesianCoordRad(lonLatH)
	{
		var cLat = Math.cos(lonLatH.lat);
		var sLat = Math.sin(lonLatH.lat);
		var cLon = Math.cos(lonLatH.lon);
		var sLon = Math.sin(lonLatH.lon);
		var e2 = this.eccentricity * this.eccentricity;
		var v = this.semiMajorAxis / Math.sqrt(1 - e2 * sLat * sLat);
		return new Vector3(
			(v + lonLatH.z) * cLat * cLon,
			(v + lonLatH.z) * cLat * sLon,
			((1 - e2) * v + lonLatH.z) * sLat);
	}

	fromCartesianCoordRad(coord)
	{
		var e2 = this.eccentricity * this.eccentricity;
		var rLon = Math.atan2(coord.y, coord.x);
		var p = Math.sqrt(coord.x * coord.x + coord.y * coord.y);
		var rLat = Math.atan2(coord.z, p * (1 - e2));
		var sLat;
		var thisLat;
		var v = 0;
		var i = 10;
		while (i-- > 0)
		{
			sLat = Math.sin(rLat);
			v = this.semiMajorAxis / Math.sqrt(1 - e2 * sLat * sLat);
			thisLat = Math.atan2(coord.z + e2 * v * sLat, p);
			if (thisLat == rLat)
				break;
			rLat = thisLat;
		}
		return new Vector3(rLon, rLat, p / Math.cos(rLat) - v);
	}

	toCartesianCoordDeg(lonLatH)
	{
		return this.toCartesianCoordRad(lonLatH.mulXY(Math.PI / 180.0));
	}

	fromCartesianCoordDeg(coord)
	{
		return this.fromCartesianCoordRad(coord).mulXY(180.0 / Math.PI);
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
	}

	equals(csys)
	{
		if (this == csys)
			return true;
		var thisType = this.getCoordSysType();
		var csysType = csys.getCoordSysType();
		if (thisType != csysType)
			return false;
		if (csysType == CoordinateSystemType.Geographic)
		{
			return this.getEllipsoid().equals(csys.getEllipsoid());
		}
		else if (cst == CoordinateSystemType.PointMapping)
		{
			return false;
		}
		else
		{
			return this.sameProjection(csys);
		}
	}

	static convert(srcCoord, destCoord, coord)
	{
		return CoordinateSystem.convert3D(srcCoord, destCoord, Vector3.fromCoord2D(coord, 0));
	}

	static convert3D(srcCoord, destCoord, srcPos)
	{
		if (srcCoord.isProjected())
		{
			srcPos = Vector3.fromCoord2D(srcCoord.toGeographicCoordinateDeg(srcPos), srcPos.z);
			srcCoord = srcCoord.getGeographicCoordinateSystem();
		}
		if (srcCoord.equals(destCoord))
		{
			return srcPos;
		}
		srcPos = srcCoord.toCartesianCoordDeg(srcPos);
	
		if (destCoord.isProjected())
		{
			var gcs = destCoord.getGeographicCoordinateSystem();
			srcPos = gcs.fromCartesianCoordRad(srcPos);
			return Vector3.fromCoord2D(destCoord.fromGeographicCoordinateRad(srcPos), srcPos.z);
		}
		else
		{
			return destCoord.fromCartesianCoordDeg(srcPos);
		}
	}

	static convertArray(srcCoord, destCoord, srcArr)
	{
		var i;
		var srcRad = false;
		var destArr = [];
		if (srcCoord.isProjected())
		{
			for (i in srcArr)
			{
				destArr[i] = srcCoord.toGeographicCoordinateRad(srcArr[i]);
			}
			srcCoord = srcCoord.getGeographicCoordinateSystem();
			srcArr = destArr;
			srcRad = true;
		}
		if (srcCoord.equals(destCoord))
		{
			if (srcRad)
			{
				for (i in srcArr)
				{
					destArr[i] = srcArr[i].mul(180.0 / Math.PI);
				}
			}
			if (srcArr != destArr)
			{
				for (i in srcArr)
				{
					destArr[i] = srcArr[i];
				}
			}
			return destArr;
		}
		var tmpPos;
		if (destCoord.isProjected())
		{
			var gcs = destCoord.getGeographicCoordinateSystem();
			if (srcRad)
			{
				for (i in srcArr)
				{
					tmpPos = srcCoord.toCartesianCoordRad(Vector3.fromCoord2D(srcArr[i], 0));
					tmpPos = gcs.fromCartesianCoordRad(tmpPos);
					destArr[i] = destCoord.fromGeographicCoordinateRad(tmpPos);
				}
			}
			else
			{
				for (i in srcArr)
				{
					tmpPos = srcCoord.toCartesianCoordDeg(Vector3.fromCoord2D(srcArr[i], 0));
					tmpPos = gcs.fromCartesianCoordRad(tmpPos);
					destArr[i] = destCoord.FromGeographicCoordinateRad(tmpPos);
				}
			}
		}
		else
		{
			if (srcRad)
			{
				for (i in srcArr)
				{
					tmpPos = srcCoord.toCartesianCoordRad(Vector3.fromCoord2D(srcArr[i], 0));
					destArr[i] = destCoord.fromCartesianCoordDeg(tmpPos);
				}
			}
			else
			{
				i = nPoints;
				while (i-- > 0)
				{
					tmpPos = srcCoord.toCartesianCoordDeg(Vector3.fromCoord2D(srcArr[i], 0));
					destArr[i] = destCoord.fromCartesianCoordDeg(tmpPos);
				}
			}
		}
		return destArr;
	}

	static convertToCartesianCoord(srcCoord, srcPos)
	{
		if (srcCoord.isProjected())
		{
			srcPos = Vector3.fromCoord2D(srcCoord.toGeographicCoordinateDeg(srcPos), srcPos.z);
			srcCoord = srcCoord.getGeographicCoordinateSystem();
		}
		return srcCoord.toCartesianCoordDeg(srcPos);
	}
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

	getCoordSysType()
	{
		return CoordinateSystemType.Geographic;
	}

	isProjected()
	{
		return false;
	}

	getEllipsoid()
	{
		return this.datum.spheroid.ellipsoid;
	}

	toCartesianCoordRad(lonLatH)
	{
		var tmpPos = this.datum.spheroid.ellipsoid.toCartesianCoordRad(lonLatH);
		if (this.datum.scale == 0 && this.datum.xAngle == 0 && this.datum.yAngle == 0 && this.datum.zAngle == 0)
		{
			return new Vector3(
				tmpPos.x + this.datum.cX,
				tmpPos.y + this.datum.cY,
				tmpPos.z + this.datum.cZ);
		}
		else
		{
			tmpPos.x -= this.datum.x0;
			tmpPos.y -= this.datum.y0;
			tmpPos.z -= this.datum.z0;
			var s = 1 + this.datum.scale * 0.000001;
			return new Vector3(
				s * (                     tmpPos.x - this.datum.zAngle * tmpPos.y + this.datum.yAngle * tmpPos.z) + this.datum.cX + this.datum.x0,
				s * ( this.datum.zAngle * tmpPos.x +                     tmpPos.y - this.datum.xAngle * tmpPos.z) + this.datum.cY + this.datum.y0,
				s * (-this.datum.yAngle * tmpPos.x + this.datum.xAngle * tmpPos.y +                     tmpPos.z) + this.datum.cZ + this.datum.z0);
		}
	}

	fromCartesianCoordRad(coord)
	{
		var tmpPos;
		if (this.datum.scale == 0 && this.datum.xAngle == 0 && this.datum.yAngle == 0 && this.datum.zAngle == 0)
		{
			tmpPos = new Vector3(
				coord.x - this.datum.cX,
				coord.y - this.datum.cY,
				coord.z - this.datum.cZ);
		}
		else
		{
			coord.x = coord.x - this.datum.x0 - this.datum.cX;
			coord.y = coord.y - this.datum.y0 - this.datum.cY;
			coord.z = coord.z - this.datum.z0 - this.datum.cZ;
			var s = 1 / (1 + this.datum.scale * 0.000001);
			tmpPos = new Vector3(
				s * (                     coord.x + this.datum.zAngle * coord.y - this.datum.yAngle * coord.z) + this.datum.x0,
				s * (-this.datum.zAngle * coord.x +                     coord.y + this.datum.xAngle * coord.z) + this.datum.y0,
				s * ( this.datum.yAngle * coord.x - this.datum.xAngle * coord.y +                     coord.z) + this.datum.z0);
		}
		return this.datum.spheroid.ellipsoid.fromCartesianCoordRad(tmpPos);
	}

	toCartesianCoordDeg(lonLatH)
	{
		return this.toCartesianCoordRad(lonLatH.mulXY(Math.PI / 180.0));
	}

	fromCartesianCoordDeg(coord)
	{
		return this.fromCartesianCoordRad(coord).mulXY(180.0 / Math.PI);
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
		if (distUnit != unit.Distance.Unit.METER)
		{
			d = unit.Distance.convert(unit.Distance.Unit.METER, distUnit, d);
		}
		return d;
	}

	isProjected()
	{
		return true;
	}

	getGeographicCoordinateSystem()
	{
		return this.gcs;
	}

	toGeographicCoordinateDeg(projPos)
	{
		return this.toGeographicCoordinateRad(projPos).mul(180 / Math.PI);
	}

	fromGeographicCoordinateDeg(geoPos)
	{
		return this.fromGeographicCoordinateRad(geoPos.mul(Math.PI / 180.0));
	}

	sameProjection(csys)
	{
		if (this.falseEasting != csys.falseEasting)
			return false;
		if (this.falseNorthing != csys.falseNorthing)
			return false;
		if (this.rcentralMeridian != csys.rcentralMeridian)
			return false;
		if (this.rlatitudeOfOrigin != csys.rlatitudeOfOrigin)
			return false;
		if (this.scaleFactor != csys.scaleFactor)
			return false;
		return this.gcs.equals(csys.gcs);
	}
}

export class MercatorProjectedCoordinateSystem extends ProjectedCoordinateSystem
{
	constructor(srid, csysName, falseEasting, falseNorthing, dcentralMeridian, dlatitudeOfOrigin, scaleFactor, gcs)
	{
		super(srid, csysName, falseEasting, falseNorthing, dcentralMeridian, dlatitudeOfOrigin, scaleFactor, gcs);
	}

	getCoordSysType()
	{
		return CoordinateSystemType.MercatorProjected;
	}

	toGeographicCoordinateRad(projPos)
	{
		var ellipsoid = this.gcs.getEllipsoid();
		var aF = ellipsoid.getSemiMajorAxis() * this.scaleFactor;
		var rLatL = (projPos.y - this.falseNorthing) / aF + this.rlatitudeOfOrigin;
		var rLastLat;
		var e = ellipsoid.getEccentricity();
		var e2 = e * e;
		var tmpV;
		var i = 20;
		while (i-- > 0)
		{
			tmpV = projPos.y - this.falseNorthing - this.calcM(rLatL);
			rLastLat = rLatL;
			rLatL = rLatL + tmpV / aF;
			if (rLastLat == rLatL || (tmpV < 0.000000001 && tmpV > -0.000000001))
				break;
		}
		var sLat = Math.sin(rLatL);
		var cLat = Math.cos(rLatL);
		var secLat = 1 / cLat;
		var tLat = sLat * secLat; //Math_Tan(rLatL);
		var tLat2 = tLat * tLat;
		var tLat4 = tLat2 * tLat2;
		var tmp = 1 - e2 * sLat * sLat;
		var v = aF / Math.sqrt(tmp);
		var v2 = v * v;
		var v3 = v * v2;
		var v5 = v3 * v2;
		var v7 = v5 * v2;
		var p = v * (1 - e2) / tmp;
		var nb2 = v / p - 1;
	
		var ser7 = tLat / (2 * p * v);
		var ser8 = tLat / (24 * p * v3) * (5 + 3 * tLat2 + nb2 - 9 * tLat2 * nb2);
		var ser9 = tLat / (720 * p * v5) * (61 + 90 * tLat2 + 45 * tLat4);
		var ser10 = secLat / v;
		var ser11 = secLat / (6 * v3) * (v / p + 2 * tLat2);
		var ser12 = secLat / (120 * v5) * (5 + 28 * tLat2 + 24 * tLat4);
		var ser12a = secLat / (5040 * v7) * (61 + 662 * tLat2 + 1320 * tLat4 + 720 * tLat4 * tLat2);
	
		var eDiff = projPos.x - this.falseEasting;
		var eDiff2 = eDiff * eDiff;
		var eDiff4 = eDiff2 * eDiff2;
		var eDiff6 = eDiff4 * eDiff2;
		return new Coord2D(this.rcentralMeridian + ser10 * eDiff - ser11 * (eDiff2 * eDiff) + ser12 * (eDiff4 * eDiff) - ser12a * (eDiff6 * eDiff),
			rLatL - ser7 * eDiff2 + ser8 * eDiff4 - ser9 * eDiff6);
	}
	
	fromGeographicCoordinateRad(geoPos)
	{
		var ellipsoid = this.gcs.getEllipsoid();
		var rLat = geoPos.lat;
		var rLon = geoPos.lon;
		var rLon0 = this.rcentralMeridian;
		var sLat = Math.sin(rLat);
		var cLat = Math.cos(rLat);
		var tLat = sLat / cLat; //Math_Tan(rLat);
		var a = ellipsoid.getSemiMajorAxis();
		var e = ellipsoid.getEccentricity();
		var e2 = e * e;
		var tmp = 1 - e2 * sLat * sLat;
		var v = a * this.scaleFactor / Math.sqrt(tmp);
		var p = v * (1 - e2) / tmp;
		var nb2 = v / p - 1;
		var m = this.calcM(rLat);
		var tLat2 = tLat * tLat;
		var tLat4 = tLat2 * tLat2;
		var cLat3 = cLat * cLat * cLat;
		var cLat5 = cLat3 * cLat * cLat;
		
		var ser1 = m + this.falseNorthing;
		var ser2 = v * 0.5 * cLat * sLat;
		var ser3 = v / 24 * sLat * cLat3 * (5 - tLat2 + 9 * nb2);
		var ser3a = v / 720 * sLat * cLat5 * (61 - 58 * tLat2 + tLat4);
		var ser4 = v * cLat;
		var ser5 = v / 6 * cLat3 * (v / p - tLat2);
		var ser6 = v / 120 * cLat5 * (5 - 18 * tLat2 + tLat4 + 14 * nb2 - 58 * tLat2 * nb2);
		var dlon = rLon - rLon0;
		var dlon2 = dlon * dlon;
		var dlon4 = dlon2 * dlon2;
	
		return new Coord2D(this.falseEasting + ser4 * dlon + ser5 * dlon * dlon2 + ser6 * dlon * dlon4,
			ser1 + ser2 * dlon2 + ser3 * dlon4 + ser3a * dlon4 * dlon2);
	}
	
	calcM(rLat)
	{
		var ellipsoid = this.gcs.getEllipsoid();
		var a = ellipsoid.getSemiMajorAxis();
		var b = ellipsoid.getSemiMinorAxis();
		var n = (a - b) / (a + b);
		var n2 = n * n;
		var n3 = n2 * n;
		var rLat0 = this.rlatitudeOfOrigin;
		var m;
		m = (1 + n + 1.25 * n2 + 1.25 * n3) * (rLat - rLat0);
		m = m - (3 * n + 3 * n2  + 2.625 * n3) * Math.sin(rLat - rLat0) * Math.cos(rLat + rLat0);
		m = m + (1.875 * n2 + 1.875 * n3) * Math.sin(2 * (rLat - rLat0)) * Math.cos(2 * (rLat + rLat0));
		m = m - 35 / 24 * n3 * Math.sin(3 * (rLat - rLat0)) * Math.cos(3 * (rLat + rLat0));
		m = m * b * this.scaleFactor;
		return m;
	}
}

export class Mercator1SPProjectedCoordinateSystem extends ProjectedCoordinateSystem
{
	constructor(srid, csysName, falseEasting, falseNorthing, dcentralMeridian, dlatitudeOfOrigin, scaleFactor, gcs)
	{
		super(srid, csysName, falseEasting, falseNorthing, dcentralMeridian, dlatitudeOfOrigin, scaleFactor, gcs);
	}

	getCoordSysType()
	{
		return CoordinateSystemType.Mercator1SPProjected;
	}

	toGeographicCoordinateRad(projPos)
	{
		var ellipsoid = this.gcs.getEllipsoid();
		var rLon0 = this.rcentralMeridian;
		var a = ellipsoid.getSemiMajorAxis();
		return new Coord2D(((projPos.x - this.falseEasting) / a + rLon0),
			(Math.atan(Math.exp((projPos.y - this.falseNorthing) / a)) - Math.PI * 0.25) * 2);
	}
	
	fromGeographicCoordinateRad(geoPos)
	{
		var ellipsoid = this.gcs.getEllipsoid();
		var rLat = geoPos.lat;
		var rLon = geoPos.lon;
		var rLon0 = this.rcentralMeridian;
		var a = ellipsoid.getSemiMajorAxis();
		var dlon = rLon - rLon0;
		return new Coord2D(this.falseEasting + dlon * a,
			this.falseNorthing + a * Math.log(Math.tan(Math.PI * 0.25 + rLat * 0.5)));
	}
}

export class CoordinateSystemManager
{
	static srCreateGeogCSysData(srid, datumSrid, name)
	{
		var data = this.srGetDatumData(datumSrid);
		if (data == null)
		{
			return null;
		}
		return new GeographicCoordinateSystem(srid, name, data);
	}
	
	static srCreateProjCSysData(srid, geogcsSrid, csysType, projName, falseEasting, falseNorthing, centralMeridian, latitudeOfOrigin, scaleFactor)
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
	
	static srCreateGeogCSys(srid)
	{
		switch (srid)
		{
		case 4326:
			return this.srCreateGeogCSysData(srid, 6326, "WGS 84");
		case 4611:
			return this.srCreateGeogCSysData(srid, 6611, "Hong Kong 1980");
		default:
			console.log("Unsupported Geog SRID: "+srid);
			return null;
		}
	}

	static srCreateProjCSys(srid)
	{
		switch (srid)
		{
		case 2326:
		case 102140:
			return this.srCreateProjCSysData(srid, 4611, CoordinateSystemType.MercatorProjected, "Hong Kong 1980 Grid System", 836694.05, 819069.80, 114.17855555555555555555555555556, 22.312133333333333333333333333333, 1);
		case 3857:
		case 900913:
			return this.srCreateProjCSysData(srid, 4326, CoordinateSystemType.Mercator1SPProjected, "WGS 84 / Pseudo-Mercator", 0, 0, 0, 0, 1);
		default:
			console.log("Unsupported Proj SRID: "+srid);
			return null;
		}
	}
	static srCreateCsys(srid)
	{
		switch (srid)
		{
		case 2326:
		case 3857:
		case 102140:
		case 900913:
			return this.srCreateProjCSys(srid);
		case 4326:
		case 4611:
			return this.srCreateGeogCSys(srid);
		default:
			console.log("Unsupported SRID: "+srid);
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
