#ifndef _SM_MAP_PROJECTIONCOORD
#define _SM_MAP_PROJECTIONCOORD
#include "Math/Ellipsoid.h"

namespace Map
{
	class ProjectionCoord
	{
	private:
		Double r;
		Double pr;
		Double flattening;
		Double originLat;
		Double originLon;
		Double falseEasting;
		Double falseNorthing;
		
	public:
		ProjectionCoord(Double originLat, Double originLon, Double radius, Double flattening, Double falseEasting, Double falseNorthing);
		~ProjectionCoord();

		void Project2Geo(Double x, Double y, Double z, Double *latDeg, Double *lonDeg, Double *h);
		void Geo2Project(Double latDeg, Double lonDeg, Double h, Double *x, Double *y, Double *z);
	};
};
#endif
