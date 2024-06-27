#ifndef _SM_MAP_SHORTESTPATH3D
#define _SM_MAP_SHORTESTPATH3D
#include "Map/MapDrawLayer.h"

namespace Map
{
	class ShortestPath3D
	{
	private:
		NN<Math::CoordinateSystem> csys;
		Double searchDist;
	public:
		ShortestPath3D(NN<Math::CoordinateSystem> csys, Double searchDist);
		~ShortestPath3D();

		void AddLayer(NN<Map::MapDrawLayer> layer);
	};
}
#endif
