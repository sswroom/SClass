#ifndef _SM_MAP_SHORTESTPATH3D
#define _SM_MAP_SHORTESTPATH3D
#include "Map/MapDrawLayer.h"
#include "Math/Geometry/LineString.h"

namespace Map
{
	class ShortestPath3D
	{
	private:
		struct LineInfo
		{
			UOSInt index;
			NN<Math::Geometry::LineString> vec;
			Math::Coord2DDbl startPos;
			Math::Coord2DDbl endPos;
			Double startZ;
			Double endZ;
			UInt32 networkId;
		};

		struct NodeInfo
		{
			Math::Coord2DDbl pos;
			Double z;
			Data::ArrayListNN<LineInfo> lines;
		};

		struct AreaInfo
		{
			OSInt x;
			OSInt y;
			Data::ArrayListNN<NodeInfo> nodes;
		};
	private:
		NN<Math::CoordinateSystem> csys;
		Double searchDist;
		Data::ArrayListNN<LineInfo> lines;
		Data::ArrayListNN<AreaInfo> areas;

		static void FreeLineInfo(NN<LineInfo> lineInfo);
		static void FreeAreaInfo(NN<AreaInfo> areaInfo);
		NN<AreaInfo> GetArea(Math::Coord2DDbl pos);
		NN<NodeInfo> GetNode(Math::Coord2DDbl pos, Double z);
		void AddVector(NN<Math::Geometry::Vector2D> vec);
	public:
		ShortestPath3D(NN<Math::CoordinateSystem> csys, Double searchDist);
		~ShortestPath3D();

		void AddLayer(NN<Map::MapDrawLayer> layer);
	};
}
#endif
