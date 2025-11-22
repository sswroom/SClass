#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.hpp"
#include "Map/GoogleMap/GoogleMapsUtil.h"

Optional<Math::Geometry::LineString> Map::GoogleMap::GoogleMapsUtil::ParsePolylineText(UnsafeArray<const UTF8Char> polylineText)
{
	Data::ArrayList<Int32> pointList;
	Int32 lastX;
	Int32 lastY;
	Int32 v;
	Int32 v2;
	UTF8Char c;
	Bool isX;
	Math::Geometry::LineString *pl;
	UOSInt i;
	UOSInt j;
	UnsafeArray<Math::Coord2DDbl> ptList;
	lastX = 0;
	lastY = 0;
	i = 0;
	v = 0;
	isX = false;
	while (true)
	{
		c = *polylineText++;
		if (c == 0)
			break;
		if (c < 63 || c > 126)
			break;
		v2 = c - 63;
		v = v | ((v2 & 0x1f) << (5 * i));
		i++;
		if ((v2 & 0x20) == 0)
		{
			 if (v & 1)
			 {
				 v = -(v >> 1) - 1;
			 }
			 else
			 {
				 v = v >> 1;
			 }
			 if (isX)
			 {
				 lastX = lastX + v;
				 pointList.Add(lastX);
				 pointList.Add(lastY);
				 isX = false;
			 }
			 else
			 {
				 lastY = lastY + v;
				 isX = true;
			 }
			 v = 0;
			 i = 0;
		}
	}
	if (v != 0 || i != 0)
		return 0;
	if (isX)
		return 0;
	j = pointList.GetCount() >> 1;
	NEW_CLASS(pl, Math::Geometry::LineString(4326, j, false, false));
	ptList = pl->GetPointList(i);
	i = 0;
	while (i < j)
	{
		ptList[i].x = pointList.GetItem((i << 1)) * 0.00001;
		ptList[i].y = pointList.GetItem((i << 1) + 1) * 0.00001;
		i++;
	}
	return pl;
}
