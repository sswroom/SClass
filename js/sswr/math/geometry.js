var geometry = {
	Vector2D: function(srid)
	{
		this.srid = srid;
	},
	
	MultiGeometry: function(srid)
	{
		geometry.Vector2D.call(this, srid);
		this.coordinates = new Array();
	},

	MultiPolygon: function(srid, coordinates)
	{
		geometry.MultiGeometry.call(this, srid);
		this.type = "MultiPolygon";
		var i = 0;
		var j = coordinates.length;
		while (i < j)
		{
			this.coordinates.push(new geometry.Polygon(srid, coordinates[i]));
			i++;
		}
	},
		
	Polygon: function(srid, coordinates)
	{
		geometry.Vector2D.call(this, srid);
		this.type = "Polygon";
		this.coordinates = coordinates;
	}
};

geometry.MultiGeometry.prototype = Object.create(geometry.Vector2D.prototype);

geometry.MultiGeometry.prototype.calBoundaryPoint = function(x, y)
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

geometry.MultiGeometry.prototype.insideVector = function(x, y)
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
geometry.MultiPolygon.prototype = Object.create(geometry.MultiGeometry.prototype);

geometry.Polygon.prototype = Object.create(geometry.Vector2D.prototype);

geometry.Polygon.prototype.calBoundaryPoint = function(x, y)
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

geometry.Polygon.prototype.insideVector = function(x, y)
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
export default geometry;