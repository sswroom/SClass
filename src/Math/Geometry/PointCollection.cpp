#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/CoordinateSystem.h"
#include "Math/Geometry/PointCollection.h"

Math::Geometry::PointCollection::PointCollection(UInt32 srid, UOSInt nPoint, const Math::Coord2DDbl *pointArr) : Vector2D(srid)
{
	this->pointArr = MemAllocA(Math::Coord2DDbl, nPoint);
	this->nPoint = nPoint;
	if (pointArr)
	{
		MemCopyAC(this->pointArr, pointArr, nPoint * sizeof(Math::Coord2DDbl));
	}
	else
	{
		MemClearAC(this->pointArr, sizeof(Math::Coord2DDbl) * nPoint);
	}
}

Math::Geometry::PointCollection::~PointCollection()
{
	MemFreeA(this->pointArr);
}

Math::Coord2DDbl Math::Geometry::PointCollection::GetCenter() const
{
	const Math::Coord2DDbl *points;
	UOSInt nPoints;

	Double maxX;
	Double maxY;
	Double minX;
	Double minY;
	Double v;
	points = this->GetPointListRead(&nPoints);
	if (nPoints <= 0)
	{
		return Math::Coord2DDbl(0, 0);
	}
	else
	{
		UOSInt i = nPoints;
		minX = maxX = points[0].x;
		minY = maxY = points[0].y;

		while (i-- > 0)
		{
			v = points[i].x;
			if (v > maxX)
			{
				maxX = v;
			}
			if (v < minX)
			{
				minX = v;
			}
			v = points[i].y;
			if (v > maxY)
			{
				maxY = v;
			}
			else if (v < minY)
			{
				minY = v;
			}
		}
		return Math::Coord2DDbl((minX + maxX) * 0.5, (minY + maxY) * 0.5);
	}
}

void Math::Geometry::PointCollection::GetBounds(Math::RectAreaDbl *bounds) const
{
	UOSInt i = this->nPoint;
	Math::Coord2DDbl min;
	Math::Coord2DDbl max;
	min = max = this->pointArr[0];
	while (i > 1)
	{
		i -= 1;
		min = min.Min(this->pointArr[i]);
		max = max.Max(this->pointArr[i]);
	}
	*bounds = Math::RectAreaDbl(min, max);
}

void Math::Geometry::PointCollection::ConvCSys(Math::CoordinateSystem *srcCSys, Math::CoordinateSystem *destCSys)
{
	Math::CoordinateSystem::ConvertXYArray(srcCSys, destCSys, this->pointArr, this->pointArr, this->nPoint);
}

