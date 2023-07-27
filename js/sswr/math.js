let math = new Object();
var sswr = new Object();
sswr.math = math;

sswr.math.roundToFloat = function(n, decimalPoints)
{
	if (decimalPoints === undefined) {
		decimalPoints = 0;
	}

	var multiplicator = Math.pow(10, decimalPoints);
	n = parseFloat((n * multiplicator).toFixed(11));
	return Math.round(n) / multiplicator;
}

sswr.math.roundToStr = function(n, decimalPoints)
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

sswr.math.geometry = new Object();
sswr.math.geometry.Vector2D = function(srid)
{
	this.srid = srid;
}

sswr.math.geometry.MultiGeometry = function(srid)
{
	sswr.math.geometry.Vector2D.call(this, srid);
	this.coordinates = new Array();
}
sswr.math.geometry.MultiGeometry.prototype = Object.create(sswr.math.geometry.Vector2D.prototype);

sswr.math.geometry.MultiGeometry.prototype.calBoundaryPoint = function(x, y)
{
	var minObj = null;
	var thisObj;
	var i = this.coordinates.length;
	while (i-- > 0)
	{
		thisObj = this.coordinates[i].calBoundaryPoint(x, y);
		if (minObj == null || minObj.dist > thisObj.dist)
		{
			minObj = thisObj;
		}
	}
	return minObj;
}

sswr.math.geometry.MultiGeometry.prototype.insideVector = function(x, y)
{
	var i = this.coordinates.length;
	while (i-- > 0)
	{
		if (this.coordinates[i].insideVector(x, y))
		{
			return true;
		}
	}
	return false;
}

sswr.math.geometry.MultiPolygon = function(srid, coordinates)
{
	sswr.math.geometry.MultiGeometry.call(this, srid);
	this.type = "MultiPolygon";
	var i = 0;
	var j = coordinates.length;
	while (i < j)
	{
		this.coordinates.push(new sswr.math.geometry.Polygon(srid, coordinates[i]));
		i++;
	}
}
sswr.math.geometry.MultiPolygon.prototype = Object.create(sswr.math.geometry.MultiGeometry.prototype);

sswr.math.geometry.Polygon = function(srid, coordinates)
{
	sswr.math.geometry.Vector2D.call(this, srid);
	this.type = "Polygon";
	this.coordinates = coordinates;
}
sswr.math.geometry.Polygon.prototype = Object.create(sswr.math.geometry.Vector2D.prototype);

sswr.math.geometry.Polygon.prototype.calBoundaryPoint = function(x, y)
{
	var k;
	var l;

	k = this.coordinates.length;
	var points;

	var calBase;
	var calDiffX;
	var calDiffY;
	var calSqDiffX;
	var calSqDiffY;
	var calPtX;
	var calPtY;
	var calPtOutX = 0;
	var calPtOutY = 0;
	var calD;
	var dist = 0x7fffffff;

	while (k--)
	{
		points = this.coordinates[k];
		l = points.length - 1;
		while (l-- > 0)
		{
			calDiffX = points[l][0] - points[l + 1][0];
			calDiffY = points[l][1] - points[l + 1][1];

			if (calDiffY == 0)
			{
				calPtX = x;
			}
			else
			{
				calSqDiffX = calDiffX * calDiffX;
				calSqDiffY = calDiffY * calDiffY;
				calBase = calSqDiffX + calSqDiffY;
				calPtX = calSqDiffX * x;
				calPtX += calSqDiffY * points[l][0];
				calPtX += (y - points[l][1]) * calDiffX * calDiffY;
				calPtX /= calBase;
			}

			if (calDiffX == 0)
			{
				calPtY = y;
			}
			else
			{
				calPtY = ((calPtX - points[l][0]) * calDiffY / calDiffX) + points[l][1];
			}

			if (calDiffX < 0)
			{
				if (points[l][0] > calPtX)
					continue;
				if (points[l + 1][0] < calPtX)
					continue;
			}
			else
			{
				if (points[l][0] < calPtX)
					continue;
				if (points[l + 1][0] > calPtX)
					continue;
			}

			if (calDiffY < 0)
			{
				if (points[l][1] > calPtY)
					continue;
				if (points[l + 1][1] < calPtY)
					continue;
			}
			else
			{
				if (points[l][1] < calPtY)
					continue;
				if (points[l + 1][1] > calPtY)
					continue;
			}

			calDiffX = x - calPtX;
			calDiffY = y - calPtY;
			calSqDiffX = calDiffX * calDiffX;
			calSqDiffY = calDiffY * calDiffY;
			calD = calSqDiffX + calSqDiffY;
			if (calD < dist)
			{
				dist = calD;
				calPtOutX = calPtX;
				calPtOutY = calPtY;
			}
		}
	}
	k = this.coordinates.length;
	while (k-- > 0)
	{
		points = this.coordinates[k];
		l = points.length;
		while (l-- > 0)
		{
			calDiffX = x - points[l][0];
			calDiffY = y - points[l][1];
			calSqDiffX = calDiffX * calDiffX;
			calSqDiffY = calDiffY * calDiffY;
			calD = calSqDiffX + calSqDiffY;
			if (calD < dist)
			{
				dist = calD;
				calPtOutX = points[l][0];
				calPtOutY = points[l][1];
			}
		}
	}
	var ret = new Object();
	ret.x = calPtOutX;
	ret.y = calPtOutY;
	ret.dist = dist;
	return ret;
}

sswr.math.geometry.Polygon.prototype.insideVector = function(x, y)
{
	var thisX;
	var thisY;
	var lastX;
	var lastY;
	var j;
	var k;
	var l;
	var leftCnt = 0;
	var tmpX;
	var points;

	k = this.coordinates.length;

	while (k--)
	{
		points = this.coordinates[k];

		lastX = points[0][0];
		lastY = points[0][1];
		l = points.length;
		while (l-- > 0)
		{
			thisX = points[l][0];
			thisY = points[l][1];
			j = 0;
			if (lastY > y)
				j += 1;
			if (thisY > y)
				j += 1;

			if (j == 1)
			{
				tmpX = lastX - (lastX - thisX) * (lastY - y) / (lastY - thisY);
				if (tmpX == x)
				{
					return true;
				}
				else if (tmpX < x)
					leftCnt++;
			}
			else if (thisY == y && lastY == y)
			{
				if ((thisX >= x && lastX <= x) || (lastX >= x && thisX <= x))
				{
					return true;
				}
			}
			else if (thisY == y && thisX == x)
			{
				return true;
			}

			lastX = thisX;
			lastY = thisY;
		}
		l++;
	}

	return (leftCnt & 1) != 0;	
}

sswr.math.unit = new Object();
sswr.math.unit.Angle = new Object();
sswr.math.unit.Angle.AngleUnit = {
	RADIAN: 1,
	GRADIAN: 2,
	TURN: 3,
	DEGREE: 4,
	ARCMINUTE: 5,
	ARCSECOND: 6,
	MILLIARCSECOND: 7,
	MICROARCSECOND: 8
};

sswr.math.unit.Angle.getUnitRatio = function(unit)
{
	switch (unit)
	{
	case sswr.math.unit.Angle.AngleUnit.RADIAN:
		return 1;
	case sswr.math.unit.Angle.AngleUnit.GRADIAN:
		return Math.PI / 200.0;
	case sswr.math.unit.Angle.AngleUnit.TURN:
		return Math.PI * 2.0;
	case sswr.math.unit.Angle.AngleUnit.DEGREE:
		return Math.PI / 180.0;
	case sswr.math.unit.Angle.AngleUnit.ARCMINUTE:
		return Math.PI / 10800.0;
	case sswr.math.unit.Angle.AngleUnit.ARCSECOND:
		return Math.PI / 648000.0;
	case sswr.math.unit.Angle.AngleUnit.MILLIARCSECOND:
		return Math.PI / 648000000.0;
	case sswr.math.unit.Angle.AngleUnit.MICROARCSECOND:
		return Math.PI / 648000000000.0;
	}
	return 1;
}

sswr.math.unit.Distance = new Object();
sswr.math.unit.Distance.DistanceUnit = {
	METER: 0,
	CENTIMETER: 1,
	MILLIMETER: 2,
	MICROMETER: 3,
	NANOMETER: 4,
	PICOMETER: 5,
	KILOMETER: 6,
	INCH: 7,
	FOOT: 8,
	YARD: 9,
	MILE: 10,
	NAUTICAL_MILE: 11,
	AU: 12,
	LIGHTSECOND: 13,
	LIGHTMINUTE: 14,
	LIGHTHOUR: 15,
	LIGHTDAY: 16,
	LIGHTWEEK: 17,
	LIGHTYEAR: 18,
	EMU: 19,
	POINT: 20,
	PIXEL: 21,
	TWIP: 22
};

sswr.math.unit.Distance.getUnitRatio = function(unit)
{
	switch (unit)
	{
	case sswr.math.unit.Distance.DistanceUnit.METER:
		return 1.0;
	case sswr.math.unit.Distance.DistanceUnit.CENTIMETER:
		return 0.01;
	case sswr.math.unit.Distance.DistanceUnit.MILLIMETER:
		return 0.001;
	case sswr.math.unit.Distance.DistanceUnit.MICROMETER:
		return 0.000001;
	case sswr.math.unit.Distance.DistanceUnit.NANOMETER:
		return 0.000000001;
	case sswr.math.unit.Distance.DistanceUnit.PICOMETER:
		return 0.000000000001;
	case sswr.math.unit.Distance.DistanceUnit.KILOMETER:
		return 1000.0;
	case sswr.math.unit.Distance.DistanceUnit.INCH:
		return 0.0254;
	case sswr.math.unit.Distance.DistanceUnit.FOOT:
		return 0.0254 * 12.0;
	case sswr.math.unit.Distance.DistanceUnit.YARD:
		return 0.0254 * 36.0;
	case sswr.math.unit.Distance.DistanceUnit.MILE:
		return 0.0254 * 12.0 * 5280;
	case sswr.math.unit.Distance.DistanceUnit.NAUTICAL_MILE:
		return 1852.0;
	case sswr.math.unit.Distance.DistanceUnit.AU:
		return 149597870700.0;
	case sswr.math.unit.Distance.DistanceUnit.LIGHTSECOND:
		return 299792458.0;
	case sswr.math.unit.Distance.DistanceUnit.LIGHTMINUTE:
		return 17987547480.0;
	case sswr.math.unit.Distance.DistanceUnit.LIGHTHOUR:
		return 299792458.0 * 3600.0;
	case sswr.math.unit.Distance.DistanceUnit.LIGHTDAY:
		return 299792458.0 * 86400.0;
	case sswr.math.unit.Distance.DistanceUnit.LIGHTWEEK:
		return 299792458.0 * 604800.0;
	case sswr.math.unit.Distance.DistanceUnit.LIGHTYEAR:
		return 299792458.0 * 31557600.0;
	case sswr.math.unit.Distance.DistanceUnit.EMU:
		return 1 / 36000000.0;
	case sswr.math.unit.Distance.DistanceUnit.POINT:
		return 0.0254 / 72.0;
	case sswr.math.unit.Distance.DistanceUnit.PIXEL:
		return 0.0254 / 96.0;
	case sswr.math.unit.Distance.DistanceUnit.TWIP:
		return 0.0254 / 1440.0;
	}
	return 1;
}

sswr.math.unit.Distance.convert = function(fromUnit, toUnit, fromValue)
{
	return fromValue * sswr.math.unit.Distance.getUnitRatio(fromUnit) / sswr.math.unit.Distance.getUnitRatio(toUnit);
}

sswr.math.CoordinateSystem = function(srid, csysName)
{
	this.srid = srid;
	this.csysName = csysName;
}

sswr.math.CoordinateSystem.prototype.CoordinateSystemType = {
	Geographic: 0,
	MercatorProjected: 1,
	Mercator1SPProjected: 2,
	PointMapping: 3,
	GausskrugerProjected: 4
};

sswr.math.CoordinateSystemManager = new Object();
sswr.math.CoordinateSystemManager.srCreateGeogCSys = function(srid, datumSrid, name)
{
	var data = this.srGetDatumData(datumSrid);
	if (data == null)
	{
		return null;
	}
	return new sswr.math.GeographicCoordinateSystem(srid, name, data);
}

sswr.math.CoordinateSystemManager.srCreateProjCSys = function(srid, geogcsSrid, csysType, projName, falseEasting, falseNorthing, centralMeridian, latitudeOfOrigin, scaleFactor)
{
	var gcsys = this.srCreateGeogCSys(geogcsSrid);
	if (gcsys == null)
		return null;
	if (csysType == sswr.math.CoordinateSystem.CoordinateSystemType.MercatorProjected || csysType == sswr.math.CoordinateSystem.CoordinateSystemType.GausskrugerProjected)
	{
		return new sswr.math.MercatorProjectedCoordinateSystem(srid, projName, falseEasting, falseNorthing, centralMeridian, latitudeOfOrigin, scaleFactor, gcsys);
	}
	else if (csysType == sswr.math.CoordinateSystem.CoordinateSystemType.Mercator1SPProjected)
	{
		return new sswr.math.Mercator1SPProjectedCoordinateSystem(srid, projName, falseEasting, falseNorthing, centralMeridian, latitudeOfOrigin, scaleFactor, gcsys);
	}
	return null;
}

sswr.math.CoordinateSystemManager.srCreateCsys = function(srid)
{
	switch (srid)
	{
	case 2326:
	case 102140:
		return this.srCreateProjCSys(srid, 4611, sswr.math.CoordinateSystem.CoordinateSystemType.MercatorProjected, "Hong Kong 1980 Grid System", 836694.05, 819069.80, 114.17855555555555555555555555556, 22.312133333333333333333333333333, 1);
	case 3857:
	case 900913:
		return this.srCreateProjCSys(srid, 4326, sswr.math.CoordinateSystem.CoordinateSystemType.Mercator1SPProjected, "WGS 84 / Pseudo-Mercator", 0, 0, 0, 0, 1);
	case 4326:
		return this.srCreateGeogCSys(srid, 6326, "WGS 84");
	case 4611:
		return this.srCreateGeogCSys(srid, 6611, "Hong Kong 1980");
	default:
		return null;
	}
}

sswr.math.CoordinateSystemManager.srGetDatumData = function(srid)
{
	switch (srid)
	{
	case 6326:
		return new sswr.math.DatumData(6326, this.srGetSpheroid(7030), "WGS_1984", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, sswr.math.unit.Angle.AngleUnit.RADIAN);
	case 6600:
		return new sswr.math.DatumData(6600, this.srGetSpheroid(7012), "Anguilla_1957", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, sswr.math.unit.Angle.AngleUnit.RADIAN);
	case 6601:
		return new sswr.math.DatumData(6601, this.srGetSpheroid(7012), "Antigua_1943", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, sswr.math.unit.Angle.AngleUnit.RADIAN);
	case 6602:
		return new sswr.math.DatumData(6602, this.srGetSpheroid(7012), "Dominica_1945", 0, 0, 0, 725, 685, 536, 0, 0, 0, 0, sswr.math.unit.Angle.AngleUnit.ARCSECOND);
	case 6603:
		return new sswr.math.DatumData(6603, this.srGetSpheroid(7012), "Grenada_1953", 0, 0, 0, 72, 213.7, 93, 0, 0, 0, 0, sswr.math.unit.Angle.AngleUnit.ARCSECOND);
	case 6611:
		return new sswr.math.DatumData(6611, this.srGetSpheroid(7022), "Hong_Kong_1980", 0, 0, 0, -162.619, -276.959, -161.764, 0.067753, -2.24365, -1.15883, -1.09425, sswr.math.unit.Angle.AngleUnit.ARCSECOND);
	}
	return null;
}

sswr.math.CoordinateSystemManager.srGetSpheroid = function(srid)
{
	switch (srid)
	{
	case 7012:
		return new sswr.math.Spheroid(7012, new sswr.math.EarthEllipsoid(null, null, sswr.math.EarthEllipsoid.EarthEllipsoidType.CLARKE1880), "Clarke 1880 (RGS)");
	case 7022:
		return new sswr.math.Spheroid(7022, new sswr.math.EarthEllipsoid(null, null, sswr.math.EarthEllipsoid.EarthEllipsoidType.INTL1924),   "International 1924");
	case 7030:
		return new sswr.math.Spheroid(7030, new sswr.math.EarthEllipsoid(null, null, sswr.math.EarthEllipsoid.EarthEllipsoidType.WGS84),      "WGS 84");
	}
}

sswr.math.DatumData = function(srid, spheroid, name, x0, y0, z0, cX, cY, cZ, xAngle, yAngle, zAngle, scale, aunit)
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
	var aratio = sswr.math.unit.Angle.getUnitRatio(aunit);
	this.xAngle = xAngle * aratio;
	this.yAngle = yAngle * aratio;
	this.zAngle = zAngle * aratio;
	this.scale = scale;
	this.aunit = sswr.math.unit.Angle.AngleUnit.RADIAN;
}

sswr.math.EarthEllipsoid = function(semiMajorAxis, inverseFlattening, eet)
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
}

sswr.math.EarthEllipsoid.EarthEllipsoidType = {
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

sswr.math.EarthEllipsoid.prototype.calSurfaceDistance = function(dLat1, dLon1, dLat2, dLon2, unit)
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
		if (unit != null && unit != sswr.math.unit.Distance.DistanceUnit.METER)
		{
			d = sswr.math.unit.Distance.convert(sswr.math.unit.Distance.DistanceUnit.METER, unit, d);
		}
	}
	else if (d != 0)
	{
		d = 0;
	}
	return d;	
}

sswr.math.EarthEllipsoid.prototype.getSemiMinorAxis = function()
{
	return this.semiMinorAxis;
}

sswr.math.EarthEllipsoid.prototype.initEarthInfo = function(eet)
{
	switch (eet)
	{
	default:
	case sswr.math.EarthEllipsoid.EarthEllipsoidType.OTHER:
		this.semiMajorAxis = 6378137.0;
		this.inverseFlattening = 191.0;
		return;
	case sswr.math.EarthEllipsoid.EarthEllipsoidType.PLESSIS:
		this.semiMajorAxis = 6376523.0;
		this.inverseFlattening = 308.64;
		return;
	case sswr.math.EarthEllipsoid.EarthEllipsoidType.EVEREST1830:
		this.semiMajorAxis = 6377299.365;
		this.inverseFlattening = 300.80172554;
		return;
	case sswr.math.EarthEllipsoid.EarthEllipsoidType.EVEREST1830M:
		this.semiMajorAxis = 6377304.063;
		this.inverseFlattening = 300.8017;
		return;
	case sswr.math.EarthEllipsoid.EarthEllipsoidType.EVEREST1830N:
		this.semiMajorAxis = 6377298.556;
		this.inverseFlattening = 300.8017;
		return;
	case sswr.math.EarthEllipsoid.EarthEllipsoidType.AIRY1830:
		this.semiMajorAxis = 6377563.396;
		this.inverseFlattening = 299.3249646;
		return;
	case sswr.math.EarthEllipsoid.EarthEllipsoidType.AIRY1830M:
		this.semiMajorAxis = 6377340.189;
		this.inverseFlattening = 299.32495141450600500090538973015;
		return;
	case sswr.math.EarthEllipsoid.EarthEllipsoidType.BESSEL1841:
		this.semiMajorAxis = 6377397.155;
		this.inverseFlattening = 299.1528128;
		return;
	case sswr.math.EarthEllipsoid.EarthEllipsoidType.CLARKE1866:
		this.semiMajorAxis = 6378206.4;
		this.inverseFlattening = 294.9786982;
		return;
	case sswr.math.EarthEllipsoid.EarthEllipsoidType.CLARKE1878:
		this.semiMajorAxis = 6378190.0;
		this.inverseFlattening = 293.4659980;
		return;
	case sswr.math.EarthEllipsoid.EarthEllipsoidType.CLARKE1880:
		this.semiMajorAxis = 6378249.145;
		this.inverseFlattening = 293.465;
		return;
	case sswr.math.EarthEllipsoid.EarthEllipsoidType.HELMERT1906:
		this.semiMajorAxis = 6378200.0;
		this.inverseFlattening = 298.3;
		return;
	case sswr.math.EarthEllipsoid.EarthEllipsoidType.HAYFORD1910:
		this.semiMajorAxis = 6378388.0;
		this.inverseFlattening = 297.0;
		return;
	case sswr.math.EarthEllipsoid.EarthEllipsoidType.INTL1924:
		this.semiMajorAxis = 6378388.0;
		this.inverseFlattening = 297.0;
		return;
	case sswr.math.EarthEllipsoid.EarthEllipsoidType.KRASSOVSKY1940:
		this.semiMajorAxis = 6378245.0;
		this.inverseFlattening = 298.3;
		return;
	case sswr.math.EarthEllipsoid.EarthEllipsoidType.WGS66:
		this.semiMajorAxis = 6378145.0;
		this.inverseFlattening = 298.25;
		return;
	case sswr.math.EarthEllipsoid.EarthEllipsoidType.AUSTRALIAN1966:
		this.semiMajorAxis = 6378160.0;
		this.inverseFlattening = 298.25;
		return;
	case sswr.math.EarthEllipsoid.EarthEllipsoidType.NEWINTL1967:
		this.semiMajorAxis = 6378157.5;
		this.inverseFlattening = 298.24961539;
		return;
	case sswr.math.EarthEllipsoid.EarthEllipsoidType.GPS67:
		this.semiMajorAxis = 6378160.0;
		this.inverseFlattening = 298.247167427;
		return;
	case sswr.math.EarthEllipsoid.EarthEllipsoidType.SAM1969:
		this.semiMajorAxis = 6378160.0;
		this.inverseFlattening = 298.25;
		return;
	case sswr.math.EarthEllipsoid.EarthEllipsoidType.WGS72:
		this.semiMajorAxis = 6378135.0;
		this.inverseFlattening = 298.26;
		return;
	case sswr.math.EarthEllipsoid.EarthEllipsoidType.GRS80:
		this.semiMajorAxis = 6378137.0;
		this.inverseFlattening = 298.257222101;
		return;
	case sswr.math.EarthEllipsoid.EarthEllipsoidType.WGS84:
		this.semiMajorAxis = 6378137.0;
		this.inverseFlattening = 298.257223563;
		return;
	case sswr.math.EarthEllipsoid.EarthEllipsoidType.WGS84_OGC:
		this.semiMajorAxis = 6378137.0;
		this.inverseFlattening = 298.257222932867;
		return;
	case sswr.math.EarthEllipsoid.EarthEllipsoidType.IERS1989:
		this.semiMajorAxis = 6378136.0;
		this.inverseFlattening = 298.257;
		return;
	case sswr.math.EarthEllipsoid.EarthEllipsoidType.IERS2003:
		this.semiMajorAxis = 6378136.6;
		this.inverseFlattening = 298.25642;
		return;
	}
}

sswr.math.GeographicCoordinateSystem = function(srid, csysName, datumData)
{
	sswr.math.CoordinateSystem.call(this, srid, csysName);
	this.datum = datumData;
}
sswr.math.GeographicCoordinateSystem.prototype = Object.create(sswr.math.CoordinateSystem.prototype);

sswr.math.GeographicCoordinateSystem.prototype.calcSurfaceDistance = function(x1, y1, x2, y2, unit)
{
	return this.datum.spheroid.ellipsoid.calSurfaceDistance(y1, x1, y2, x2, unit);
}

sswr.math.GeoJSON = new Object();
sswr.math.GeoJSON.parseGeometry = function(srid, geometry)
{
	if (geometry.type == "Polygon")
	{
		return new sswr.math.geometry.Polygon(srid, geometry.coordinates);
	}
	else if (geometry.type == "MultiPolygon")
	{
		return new sswr.math.geometry.MultiPolygon(srid, geometry.coordinates);
	}
	else
	{
		console.log("GeoJSON.parseGeometry: unknown type "+geometry.type);
	}
	return null;
}

sswr.math.ProjectedCoordinateSystem = function(srid, csysName, falseEasting, falseNorthing, dcentralMeridian, dlatitudeOfOrigin, scaleFactor, gcs)
{
	sswr.math.CoordinateSystem.call(this, srid, csysName);
	this.falseEasting = falseEasting;
	this.falseNorthing = falseNorthing;
	this.rcentralMeridian = dcentralMeridian * Math.PI / 180;
	this.rlatitudeOfOrigin = dlatitudeOfOrigin * Math.PI / 180;
	this.scaleFactor = scaleFactor;
	this.gcs = gcs;
}
sswr.math.ProjectedCoordinateSystem.prototype = Object.create(sswr.math.CoordinateSystem.prototype);

sswr.math.ProjectedCoordinateSystem.prototype.calcSurfaceDistance = function(x1, y1, x2, y2, unit)
{
	var diffX = x2 - x1;
	var diffY = y2 - y1;
	diffX = diffX * diffX;
	diffY = diffY * diffY;
	var d = Math.sqrt(diffX + diffY);
	if (unit != sswr.math.unit.Distance.DistanceUnit.METER)
	{
		d = sswr.math.unit.Distance.convert(sswr.math.unit.Distance.DistanceUnit.METER, unit, d);
	}
	return d;
}

sswr.math.MercatorProjectedCoordinateSystem = function(srid, csysName, falseEasting, falseNorthing, dcentralMeridian, dlatitudeOfOrigin, scaleFactor, gcs)
{
	sswr.math.ProjectedCoordinateSystem.call(this, srid, csysName, falseEasting, falseNorthing, dcentralMeridian, dlatitudeOfOrigin, scaleFactor, gcs);
}
sswr.math.MercatorProjectedCoordinateSystem.prototype = Object.create(sswr.math.ProjectedCoordinateSystem.prototype);

sswr.math.Spheroid = function(srid, ellipsoid, name)
{
	this.srid = srid;
	this.ellipsoid = ellipsoid;
	this.name = name;
}

export default math;