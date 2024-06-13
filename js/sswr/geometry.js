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
	/**
	 * @param {number} srid
	 */
	constructor(srid)
	{
		this.srid = srid;
	}
}

export class Point extends Vector2D
{
	/**
	 * @param {number} srid
	 * @param {math.Vector3 | math.Coord2D} coordinates
	 */
	constructor(srid, coordinates)
	{
		super(srid);
		this.type = VectorType.Point;
		if (coordinates instanceof math.Vector3)
			this.coordinates = [coordinates.x, coordinates.y, coordinates.z];
		else if (coordinates instanceof math.Coord2D)
			this.coordinates = [coordinates.x, coordinates.y];
		else
			this.coordinates = coordinates;
	}

	/**
	 * @param {math.Coord2D} coord
	 */
	insideOrTouch(coord)
	{
		return this.coordinates[0] == coord.x && this.coordinates[1] == coord.y;
	}

	getBounds()
	{
		return new math.RectArea(this.coordinates[0], this.coordinates[1], this.coordinates[0], this.coordinates[1]);
	}
}

export class LineString extends Vector2D
{
	/**
	 * @param {number} srid
	 * @param {number[][]} coordinates
	 */
	constructor(srid, coordinates)
	{
		super(srid);
		this.type = VectorType.LineString;
		this.coordinates = coordinates;
	}

	/**
	 * @param {math.Coord2D} coord
	 */
	calBoundaryPoint(coord)
	{
		let l;
		let points;
	
		let calBase;
		let calDiffX;
		let calDiffY;
		let calSqDiffX;
		let calSqDiffY;
		let calPtX;
		let calPtY;
		let calPtOutX = 0;
		let calPtOutY = 0;
		let calD;
		let dist = 0x7fffffff;
		let x = coord.x;
		let y = coord.y;
	
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
		return {
			x: calPtOutX,
			y: calPtOutY,
			dist: Math.sqrt(dist)
		};
	}

	/**
	 * @param {math.Coord2D} coord
	 */
	insideOrTouch(coord)
	{
		let thisX;
		let thisY;
		let lastX;
		let lastY;
		let j;
		let l;
		let tmpX;
	
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

	getBounds()
	{
		let bounds = new math.RectArea(this.coordinates[0][0], this.coordinates[0][1], this.coordinates[0][0], this.coordinates[0][1]);
		let i;
		for (i in this.coordinates)
		{
			bounds.unionPointInPlace(this.coordinates[i][0], this.coordinates[i][1]);
		}
		return bounds;
	}
}

export class LinearRing extends LineString
{
	/**
	 * @param {number} srid
	 * @param {number[][]} coordinates
	 */
	constructor(srid, coordinates)
	{
		super(srid, coordinates);
		this.type = VectorType.LinearRing;
	}

	/**
	 * @param {math.Coord2D} coord
	 */
	insideOrTouch(coord)
	{
		let thisX;
		let thisY;
		let lastX;
		let lastY;
		let j;
		let l;
		let leftCnt = 0;
		let tmpX;
		let points;
		let x = coord.x;
		let y = coord.y;
	
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
		let firstPoint = this.coordinates[0];
		let lastPoint = this.coordinates[this.coordinates.length - 1];
		return firstPoint[0] == lastPoint[0] && firstPoint[1] == lastPoint[1];
	}

	toPolygon()
	{
		return new Polygon(this.srid, [this.coordinates]);
	}

	/**
	 * @param {number} srid
	 * @param {math.Coord2D} center
	 * @param {number} radiusX
	 * @param {number} radiusY
	 * @param {number} nPoints
	 */
	static createFromCircle(srid, center, radiusX, radiusY, nPoints)
	{
		let pos = [];
		let ratio = 2 * Math.PI / nPoints;
		let i = 0;
		let angle;
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
	/**
	 * @param {number} srid
	 */
	constructor(srid)
	{
		super(srid);
		this.geometries = new Array();
	}

	/**
	 * @param {math.Coord2D} coord
	 */
	calBoundaryPoint(coord)
	{
		let minObj = null;
		let thisObj;
		let i = this.geometries.length;
		while (i-- > 0)
		{
			thisObj = this.geometries[i].calBoundaryPoint(coord);
			if (minObj == null || minObj.dist > thisObj.dist)
			{
				minObj = thisObj;
			}
		}
		return minObj;
	}

	/**
	 * @param {math.Coord2D} coord
	 */
	insideOrTouch(coord)
	{
		let i = this.geometries.length;
		while (i-- > 0)
		{
			if (this.geometries[i].insideOrTouch(coord))
			{
				return true;
			}
		}
		return false;
	}

	getBounds()
	{
		if (this.geometries.length == 0)
		{
			return null;
		}
		let bounds;
		let thisBounds;
		let i;
		for (i in this.geometries)
		{
			thisBounds = this.geometries[i].getBounds();
			if (thisBounds)
			{
				if (bounds)
					bounds.unionInPlace(thisBounds);
				else
					bounds = thisBounds;
			}
		}
		return bounds;
	}
}

export class Polygon extends MultiGeometry
{
	/**
	 * @param {number} srid
	 * @param {number[][][] | undefined} coordinates
	 */
	constructor(srid, coordinates)
	{
		super(srid);
		this.type = VectorType.Polygon;
		if (coordinates)
		{
			let i;
			for (i in coordinates)
			{
				this.geometries.push(new LinearRing(srid, coordinates[i]));
			}
		}
	}

	/**
	 * @param {math.Coord2D} coord
	 */
	insideOrTouch(coord)
	{
		let i = this.geometries.length;
		let inside = false;
		while (i-- > 0)
		{
			if (this.geometries[i].insideOrTouch(coord))
			{
				inside = !inside;
			}
		}
		return inside;
	}
}

export class MultiPolygon extends MultiGeometry
{
	/**
	 * @param {number} srid
	 * @param {number[][][][]} coordinates
	 */
	constructor(srid, coordinates)
	{
		super(srid);
		this.type = VectorType.MultiPolygon;
		let i;
		if (coordinates)
		{
			for (i in coordinates)
			{
				this.geometries.push(new Polygon(srid, coordinates[i]));
			}
		}
	}
}

export class Polyline extends MultiGeometry
{
	/**
	 * @param {number} srid
	 * @param {number[][][]} coordinates
	 */
	constructor(srid, coordinates)
	{
		super(srid);
		this.type = VectorType.Polyline;
		let i;
		if (coordinates)
		{
			for (i in coordinates)
			{
				this.geometries.push(new LineString(srid, coordinates[i]));
			}
		}
	}
}

export class GeometryCollection extends MultiGeometry
{
	/**
	 * @param {number} srid
	 */
	constructor(srid)
	{
		super(srid);
		this.type = VectorType.GeometryCollection;
	}
}
