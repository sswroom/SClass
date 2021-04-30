#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/PointCollection.h"

Math::PointCollection::PointCollection(UInt32 srid) : Vector2D(srid)
{

}

Math::PointCollection::~PointCollection()
{

}

void Math::PointCollection::GetCenter(Double *x, Double *y)
{
	Double *points;
	UOSInt nPoints;

	Double maxX;
	Double maxY;
	Double minX;
	Double minY;
	Double v;
	points = this->GetPointList(&nPoints);
	if (nPoints <= 0)
	{
		*x = 0;
		*y = 0;
	}
	else
	{
		UOSInt i = nPoints;
		minX = maxX = points[0];
		minY = maxY = points[1];

		while (i-- > 0)
		{
			v = points[(i << 1)];
			if (v > maxX)
			{
				maxX = v;
			}
			if (v < minX)
			{
				minX = v;
			}
			v = points[(i << 1) + 1];
			if (v > maxY)
			{
				maxY = v;
			}
			else if (v < minY)
			{
				minY = v;
			}
		}
		*x = (minX + maxX) * 0.5;
		*y = (minY + maxY) * 0.5;
	}
}
