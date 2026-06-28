#ifndef _SM_MAP_SHORTESTPATHFINDER
#define _SM_MAP_SHORTESTPATHFINDER
#include "Data/FastMapNN.hpp"
#include "Map/MapDrawLayer.h"
#include "Math/Geometry/Polyline.h"

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
			UnsafeArrayOpt<const UTF8Char> name;
			NN<Math::Geometry::Polyline> pl;
			Bool isReversed;
		} NeighbourInfo;

		typedef struct
		{
			Double x;
			Double y;
			NN<Data::ArrayListNN<NeighbourInfo>> neighbours;
		} NodeInfo;
	private:
		NN<Map::MapDrawLayer> layer;
		Optional<Map::NameArray> nameArr;
		Bool toRelease;
		IntOS nameCol;
		IntOS dirCol;
		CoordinateUnit cu;
		NN<Data::Int64FastMapNN<NodeInfo>> nodeMap;

		Int64 CoordToId(Double x, Double y);
		Bool SearchShortestPath(NN<Data::ArrayListNative<Double>> points, NN<Map::GetObjectSess> sess, Int64 fromObjId, Double fromX, Double fromY, Int64 toObjId, Double toX, Double toY);
	public:
		ShortestPathFinder(NN<Map::MapDrawLayer> layer, Bool toRelease, CoordinateUnit cu, IntOS nameCol, IntOS dirCol);
		~ShortestPathFinder();

		Optional<Math::Geometry::LineString> GetPath(Double fromX, Double fromY, Double toX, Double toY, Bool sameName);
	};
}
#endif
