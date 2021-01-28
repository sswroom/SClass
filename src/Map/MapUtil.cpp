#include "Stdafx.h"
#include "Map/MapUtil.h"
#include <math.h>

#define PI 3.141592653589793

Double Map::MapUtil::CalDistance(Double lat1, Double lon1, Double lat2, Double lon2)
{
    Double r;
    Double rLat1;
    Double rLon1;
    Double rLat2;
    Double rLon2;
    
	if (lat1 == lat2 && lon1 == lon2)
		return 0;

    r = 6378000.0;
    rLat1 = lat1 * PI / 180.0;
    rLon1 = lon1 * PI / 180.0;
    rLat2 = lat2 * PI / 180.0;
    rLon2 = lon2 * PI / 180.0;

    return acos(cos(rLat1) * cos(rLon1) * cos(rLat2) * cos(rLon2) + cos(rLat1) * sin(rLon1) * cos(rLat2) * sin(rLon2) + sin(rLat1) * sin(rLat2)) * r;
}

Int32 Map::MapUtil::SplitPoints(Double *buff, Double lat1, Double lon1, Double lat2, Double lon2, Double dist)
{
	Double totalDist = CalDistance(lat1, lon1, lat2, lon2);
	Int32 pts = 1;
	Double intDist = dist * 1.5;
	Double distLeft = totalDist;
	Double currDist = 0;
	*buff++ = lat1;
	*buff++ = lon1;
	if (totalDist == 0)
	{
		*buff++ = lat2;
		*buff++ = lon2;
		pts++;
		return pts;
	}
	while (distLeft > intDist)
	{
		currDist += dist;
		distLeft -= dist;
		*buff++ = lat1 + (lat2 - lat1) * currDist / totalDist;
		*buff++ = lon1 + (lon2 - lon1) * currDist / totalDist;
		pts++;
	}
	*buff++ = lat2;
	*buff++ = lon2;
	pts++;
	return pts;
}

Double Map::MapUtil::PointDir(Double lat1, Double lon1, Double lat2, Double lon2)
{
	Double x;
	Double y;
	y = lat2 - lat1;
	x = lon2 - lon1;
	x = 90 + atan2(y, x) * 180 / PI;
	if (x < 0)
		x += 360;
	return x;
}
