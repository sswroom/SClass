#ifndef _SM_MAP_SHORTESTPATH3D
#define _SM_MAP_SHORTESTPATH3D
#include "Map/MapDrawLayer.h"
#include "Math/Geometry/LineString.h"
#include "Sync/Mutex.h"

namespace Map
{
	class ShortestPath3D
	{
	public:
		struct LineInfo
		{
			UOSInt index;
			NN<Math::Geometry::LineString> vec;
			Math::Coord2DDbl startPos;
			Math::Coord2DDbl endPos;
			Double startZ;
			Double endZ;
			Double length;
			UInt32 networkId;
		};

		struct NodeInfo
		{
			Math::Coord2DDbl pos;
			Double z;
			Data::ArrayListNN<LineInfo> lines;
			UInt32 networkId;
			Double calcNodeDist;
			Math::Coord2DDbl calcFrom;
			Double calcFromZ;
			NN<Math::Geometry::LineString> calcLine;
		};

		struct AreaInfo
		{
			OSInt x;
			OSInt y;
			Data::ArrayListNN<NodeInfo> nodes;
		};

		struct PathResult
		{
			Math::Coord2DDbl point;
			Double dist;
			NN<LineInfo> line;
			NN<Math::Geometry::LineString> vec;
		};

	private:
		class LineComparator : public Data::Comparator<NN<LineInfo>>
		{
		public:
			LineComparator();
			virtual ~LineComparator();

			virtual OSInt Compare(NN<LineInfo> a, NN<LineInfo> b) const;
		};

		class PathResultComparator : public Data::Comparator<NN<PathResult>>
		{
		public:
			PathResultComparator();
			virtual ~PathResultComparator();

			virtual OSInt Compare(NN<PathResult> a, NN<PathResult> b) const;
		};

		class NodeDistanceComparator : public Data::Comparator<NN<NodeInfo>>
		{
		public:
			NodeDistanceComparator();
			virtual ~NodeDistanceComparator();

			virtual OSInt Compare(NN<NodeInfo> a, NN<NodeInfo> b) const;
		};

		NN<Math::CoordinateSystem> csys;
		Double searchDist;
		Data::ArrayListNN<LineInfo> lines;
		Data::ArrayListNN<AreaInfo> areas;
		Sync::Mutex mut;

		static void FreeLineInfo(NN<LineInfo> lineInfo);
		static void FreeAreaInfo(NN<AreaInfo> areaInfo);
		static void AddAreaLines(NN<Data::ArrayListNN<LineInfo>> lines, NN<AreaInfo> areaInfo);
		NN<AreaInfo> GetArea(Math::Coord2DDbl pos);
		Optional<AreaInfo> GetExistingArea(OSInt areaX, OSInt areaY);
		NN<NodeInfo> GetNode(Math::Coord2DDbl pos, Double z);
		void FillNetwork(NN<NodeInfo> nodeInfo, UInt32 networkId);
		void AddVector(NN<Math::Geometry::Vector2D> vec);
	public:
		ShortestPath3D(NN<Math::CoordinateSystem> csys, Double searchDist);
		~ShortestPath3D();

		void AddLayer(NN<Map::MapDrawLayer> layer);
		void BuildNetwork();
		void GetNetworkLines(NN<Data::ArrayListNN<Math::Geometry::LineString>> lines, UInt32 networkId);
		void GetNearestPaths(NN<Data::ArrayListNN<PathResult>> paths, Math::Coord2DDbl pos);
		Optional<Math::Geometry::LineString> GetShortestPath(Math::Coord2DDbl posStart, Math::Coord2DDbl posEnd);
		NN<Math::CoordinateSystem> GetCoordinateSystem() const;
	};
}
#endif
