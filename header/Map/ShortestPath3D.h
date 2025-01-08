#ifndef _SM_MAP_SHORTESTPATH3D
#define _SM_MAP_SHORTESTPATH3D
#include "Data/ArrayListT.h"
#include "Data/DataArray.h"
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
			Math::RectAreaDbl rect;
			Math::Coord2DDbl startPos;
			Math::Coord2DDbl endPos;
			Double startZ;
			Double endZ;
			Double length;
			UInt32 networkId;
			Data::DataArray<Optional<Text::String>> properties;
		};

		struct NodeInfo
		{
			Math::Coord2DDbl pos;
			Double z;
			Data::ArrayListNN<LineInfo> lines;
			UInt32 networkId;
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

		struct NodeSession
		{
			NN<NodeInfo> node;
			Double calcNodeDist;
			Math::Coord2DDbl calcFrom;
			Double calcFromZ;
			NN<Math::Geometry::LineString> calcLine;
			Data::DataArray<Optional<Text::String>> calcLineProp;
		};

		struct AreaSession
		{
			OSInt x;
			OSInt y;
			Data::ArrayListNN<NodeSession> nodes;
		};

		struct PathSession
		{
			Optional<Math::Geometry::LineString> lastStartHalfLine1;
			Optional<Math::Geometry::LineString> lastStartHalfLine2;
			Optional<Math::Geometry::LineString> lastEndHalfLine1;
			Optional<Math::Geometry::LineString> lastEndHalfLine2;
			Data::ArrayListNN<AreaSession> areas;
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

		class NodeDistanceComparator : public Data::Comparator<NN<NodeSession>>
		{
		public:
			NodeDistanceComparator();
			virtual ~NodeDistanceComparator();

			virtual OSInt Compare(NN<NodeSession> a, NN<NodeSession> b) const;
		};

		NN<Math::CoordinateSystem> csys;
		Double searchDist;
		Data::ArrayListNN<LineInfo> lines;
		Data::ArrayListNN<AreaInfo> areas;
		UInt32 networkCnt;
		Optional<DB::TableDef> propDef;
		NN<NodeInfo> unknownNode;

		static void __stdcall FreeLineInfo(NN<LineInfo> lineInfo);
		static void __stdcall FreeAreaInfo(NN<AreaInfo> areaInfo) { areaInfo->nodes.FreeAll(FreeNodeInfo); areaInfo.Delete(); }
		static void __stdcall FreeNodeInfo(NN<NodeInfo> nodeInfo) { nodeInfo.Delete(); }
		static void __stdcall FreeNodeSess(NN<NodeSession> nodeSess) { nodeSess.Delete(); }
		static void __stdcall FreeAreaSess(NN<AreaSession> areaSess) { areaSess->nodes.FreeAll(FreeNodeSess); areaSess.Delete(); }
		static void AddAreaLines(NN<Data::ArrayListNN<LineInfo>> lines, NN<AreaInfo> areaInfo);
		NN<AreaInfo> GetArea(Math::Coord2DDbl pos);
		Optional<AreaInfo> GetAreaOpt(Math::Coord2DDbl pos) const;
		NN<AreaSession> GetAreaSess(NN<PathSession> sess, Math::Coord2DDbl pos) const;
		Optional<AreaInfo> GetExistingArea(OSInt areaX, OSInt areaY) const;
		NN<NodeInfo> GetNode(Math::Coord2DDbl pos, Double z);
		NN<NodeInfo> GetNodeOrUnknown(Math::Coord2DDbl pos, Double z) const;
		NN<NodeSession> GetNodeSess(NN<PathSession> sess, Math::Coord2DDbl pos, Double z) const;
		void FillNetwork(NN<NodeInfo> nodeInfo, UInt32 networkId);
		void AddVector(NN<Math::Geometry::Vector2D> vec, Data::DataArray<Optional<Text::String>> properties);
	public:
		ShortestPath3D(NN<Math::CoordinateSystem> csys, Double searchDist);
		ShortestPath3D(NN<Map::MapDrawLayer> layer, Double searchDist);
		~ShortestPath3D();

		void AddLayer(NN<Map::MapDrawLayer> layer);
		void BuildNetwork();
		void GetNetworkLines(NN<Data::ArrayListNN<Math::Geometry::LineString>> lines, UInt32 networkId) const;
		void GetNearestPaths(NN<Data::ArrayListNN<PathResult>> paths, Math::Coord2DDbl pos) const;
		NN<PathSession> CreateSession() const;
		void FreeSession(NN<PathSession> sess) const;
		Bool GetShortestPathDetail(NN<PathSession> sess, Math::Coord2DDbl posStart, Math::Coord2DDbl posEnd, NN<Data::ArrayListNN<Math::Geometry::LineString>> lineList, NN<Data::ArrayListT<Data::DataArray<Optional<Text::String>>>> propList) const;
		Optional<Math::Geometry::LineString> GetShortestPath(NN<PathSession> sess, Math::Coord2DDbl posStart, Math::Coord2DDbl posEnd) const;
		NN<Math::CoordinateSystem> GetCoordinateSystem() const;
		Optional<DB::TableDef> GetPropDef() const;
		UInt32 GetNetworkCnt() const;
		void GetLines(Math::RectAreaDbl rect, NN<Data::ArrayListNN<Math::Geometry::LineString>> lineList) const;

		static void CalcDirReverse(NN<Data::ArrayListNN<Math::Geometry::LineString>> lineList, NN<Data::ArrayList<Double>> dirList, NN<Data::ArrayList<Bool>> reverseList);
	};
}
#endif
