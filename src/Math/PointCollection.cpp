#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/PointCollection.h"

Math::PointCollection::PointCollection(UInt32 srid) : Vector2D(srid)
{

}

Math::PointCollection::~PointCollection()
{

}

Math::Coord2DDbl Math::PointCollection::GetCenter()
{
	Math::Coord2DDbl *points;
	UOSInt nPoints;

	Double maxX;
	Double maxY;
	Double minX;
	Double minY;
	Double v;
	points = this->GetPointList(&nPoints);
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
