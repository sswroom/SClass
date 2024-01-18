import * as math from "./math.js";

export const VectorType = {
	Unknown: "Unknown",
	Point: "Point",
	LineString: "LineString",
	Polygon: "Polygon",
	MultiPoint: "MultiPoint",
	Polyline: "MultiLineString",
	MultiPolygon: "MultiPolygon",
	GeometryCollection: "GeometryCollection",
	CircularString: "CircularString",
	CompoundCurve: "CompoundCurve",
	CurvePolygon: "CurvePolygon",
	MultiCurve: "MultiCurve",
	MultiSurface: "MultiSurface",
	Curve: "Curve",
	Surface: "Surface",
	PolyhedralSurface: "PolyhedralSurface",
	Tin: "Tin",
	Triangle: "Triange",
	LinearRing: "LinearRing",

	Image: "Image",
	String: "String",
	Ellipse: "Ellipse",
	PieArea: "PieArea"
}

export class Vector2D {
	constructor(srid)
	{
		this.srid = srid;
	}
}

export class Point extends Vector2D
{
	constructor(srid, coordinates)
	{
		super(srid);
		this.type = VectorType.Point;
		if (coordinates instanceof math.Coord2D)
			this.coordinates = [coordinates.x, coordinates.y];
		else if (coordinates instanceof math.Vector3)
			this.coordinates = [coordinates.x, coordinates.y, coordinates.z];
		else
			this.coordinates = coordinates;
	}

	insideOrTouch(coord)
	{
		return this.coordinates[0] == coord.x && this.coordinates[1] == coord.y;
	}
}

export class LineString extends Vector2D
{
	constructor(srid, coordinates)
	{
		super(srid);
		this.type = VectorType.LineString;
		this.coordinates = coordinates;
	}

	calBoundaryPoint(coord)
	{
		var l;
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
		var x = coord.x;
		var y = coord.y;
	
		points = this.coordinates;
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
		points = this.coordinates;
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
		var ret = new Object();
		ret.x = calPtOutX;
		ret.y = calPtOutY;
		ret.dist = Math.sqrt(dist);
		return ret;
	}

	insideOrTouch(coord)
	{
		var thisX;
		var thisY;
		var lastX;
		var lastY;
		var j;
		var l;
		var tmpX;
	
		l = this.coordinates.length;
		lastX = this.coordinates[0][0];
		lastY = this.coordinates[0][1];
		while (l-- > 0)
		{
			thisX = this.coordinates[l][0];
			thisY = this.coordinates[l][1];
			j = 0;
			if (lastY > coord.y)
				j += 1;
			if (thisY > coord.y)
				j += 1;
	
			if (j == 1)
			{
				tmpX = lastX - (lastX - thisX) * (lastY - coord.y) / (lastY - thisY);
				if (tmpX == coord.x)
				{
					return true;
				}
			}
			else if (thisY == coord.y && lastY == coord.y)
			{
				if ((thisX >= coord.x && lastX <= coord.x) || (lastX >= coord.x && thisX <= coord.x))
				{
					return true;
				}
			}
			else if (thisY == coord.y && thisX == coord.x)
			{
				return true;
			}
	
			lastX = thisX;
			lastY = thisY;
		}
		return false;
	}
}

export class LinearRing extends LineString
{
	constructor(srid, coordinates)
	{
		super(srid, coordinates);
		this.type = VectorType.LinearRing;
	}

	insideOrTouch(coord)
	{
		var thisX;
		var thisY;
		var lastX;
		var lastY;
		var j;
		var l;
		var leftCnt = 0;
		var tmpX;
		var points;
		var x = coord.x;
		var y = coord.y;
	
		points = this.coordinates;

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
	
		return (leftCnt & 1) != 0;	
	}

	isOpen()
	{
		return !this.isClose();
	}

	isClose()
	{
		var firstPoint = this.coordinates[0];
		var lastPoint = this.coordinates[this.coordinates.length - 1];
		return firstPoint[0] == lastPoint[0] && firstPoint[1] == lastPoint[1];
	}

	toPolygon()
	{
		return new Polygon(this.srid, [this.coordinates]);
	}

	static createFromCircle(srid, center, radiusX, radiusY, nPoints)
	{
		var pos = [];
		var ratio = 2 * Math.PI / nPoints;
		var i = 0;
		var angle;
		i = 0;
		while (i <= nPoints)
		{
			angle = i * ratio;
			pos.push([center.x + radiusX * Math.cos(angle), center.y + radiusY * Math.sin(angle)]);
			i++;
		}
		return new LinearRing(srid, pos);
	}
}

export class MultiGeometry extends Vector2D
{
	constructor(srid)
	{
		super(srid);
		this.geometries = new Array();
	}

	calBoundaryPoint(coord)
	{
		var minObj = null;
		var thisObj;
		var i = this.coordinates.length;
		while (i-- > 0)
		{
			thisObj = this.coordinates[i].calBoundaryPoint(coord);
			if (minObj == null || minObj.dist > thisObj.dist)
			{
				minObj = thisObj;
			}
		}
		return minObj;
	}

	insideOrTouch(coord)
	{
		var i = this.coordinates.length;
		while (i-- > 0)
		{
			if (this.coordinates[i].insideOrTouch(coord))
			{
				return true;
			}
		}
		return false;
	}
}

export class Polygon extends MultiGeometry
{
	constructor(srid, coordinates)
	{
		super(srid);
		this.type = VectorType.Polygon;
		var i;
		for (i in coordinates)
		{
			this.geometries.push(new LinearRing(srid, coordinates[i]));
		}
	}

	insideOrTouch(coord)
	{
		var i = this.coordinates.length;
		var inside = false;
		while (i-- > 0)
		{
			if (this.coordinates[i].insideOrTouch(coord))
			{
				inside = !inside;
			}
		}
		return inside;
	}
}

export class MultiPolygon extends MultiGeometry
{
	constructor(srid, coordinates)
	{
		super(srid);
		this.type = VectorType.MultiPolygon;
		var i;
		for (i in coordinates)
		{
			this.geometries.push(new Polygon(srid, coordinates[i]));
		}
	}
}
