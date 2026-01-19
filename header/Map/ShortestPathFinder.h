#ifndef _SM_MAP_SHORTESTPATHFINDER
#define _SM_MAP_SHORTESTPATHFINDER
#include "Data/Int64Map.h"
#include "Map/MapDrawLayer.h"
#include "Math/Polyline.h"

namespace Map
{
	class ShortestPathFinder
	{
	public:
		typedef enum
		{
			CU_DEGREE,
			CU_METER
		} CoordinateUnit;

		typedef struct
		{
			Double x;
			Double y;
			Double dist;
			const UTF8Char *name;
			Math::Polyline *pl;
			Bool isReversed;
		} NeighbourInfo;

		typedef struct
		{
			Double x;
			Double y;
			Optional<Data::ArrayListNN<NeighbourInfo>> neighbours;
		} NodeInfo;
	private:
		NN<Map::MapDrawLayer> layer;
		void *nameArr;
		Bool toRelease;
		IntOS nameCol;
		IntOS dirCol;
		CoordinateUnit cu;
		NN<Data::Int64MapNN<NodeInfo>> nodeMap;

		Int64 CoordToId(Double x, Double y);
		Bool SearchShortestPath(NN<Data::ArrayList<Double>> points, void *sess, Int64 fromObjId, Double fromX, Double fromY, Int64 toObjId, Double toX, Double toY);
	public:
		ShortestPathFinder(NN<Map::MapDrawLayer> layer, Bool toRelease, CoordinateUnit cu, IntOS nameCol, IntOS dirCol);
		~ShortestPathFinder();

		Optional<Math::Polyline> GetPath(Double fromX, Double fromY, Double toX, Double toY, Bool sameName);
	};
}
#endif
