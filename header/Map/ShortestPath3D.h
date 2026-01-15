#ifndef _SM_MAP_SHORTESTPATH3D
#define _SM_MAP_SHORTESTPATH3D
#include "Data/ArrayListT.hpp"
#include "Data/DataArray.hpp"
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
			UIntOS index;
			NN<Math::Geometry::LineString> vec;
			Math::RectAreaDbl rect;
			Math::Coord2DDbl startPos;
			Math::Coord2DDbl endPos;
			Double startZ;
			Double endZ;
			Double length;
			UInt32 networkId;
			Bool allowReverse;
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
			IntOS x;
			IntOS y;
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
			IntOS x;
			IntOS y;
			Data::ArrayListNN<NodeSession> nodes;
		};

		struct PathSession
		{
			Optional<Math::Geometry::LineString> lastStartHalfLine1;
			Optional<Math::Geometry::LineString> lastStartHalfLine2;
			Optional<Math::Geometry::LineString> lastEndHalfLine1;
			Optional<Math::Geometry::LineString> lastEndHalfLine2;
			Data::ArrayListNN<AreaSession> areas;
			Int64 lastUpdated;
		};

	private:
		class LineComparator : public Data::Comparator<NN<LineInfo>>
		{
		public:
			LineComparator();
			virtual ~LineComparator();

			virtual IntOS Compare(NN<LineInfo> a, NN<LineInfo> b) const;
		};

		class PathResultComparator : public Data::Comparator<NN<PathResult>>
		{
		public:
			PathResultComparator();
			virtual ~PathResultComparator();

			virtual IntOS Compare(NN<PathResult> a, NN<PathResult> b) const;
		};

		class NodeDistanceComparator : public Data::Comparator<NN<NodeSession>>
		{
		public:
			NodeDistanceComparator();
			virtual ~NodeDistanceComparator();

			virtual IntOS Compare(NN<NodeSession> a, NN<NodeSession> b) const;
		};

		NN<Math::CoordinateSystem> csys;
		Double searchDist;
		Data::ArrayListNN<LineInfo> lines;
		Data::ArrayListNN<AreaInfo> areas;
		UInt32 networkCnt;
		Optional<DB::TableDef> propDef;
		NN<NodeInfo> unknownNode;
		Int64 lastUpdated;

		static void __stdcall FreeLineInfo(NN<LineInfo> lineInfo);
		static void __stdcall FreeAreaInfo(NN<AreaInfo> areaInfo) { areaInfo->nodes.FreeAll(FreeNodeInfo); areaInfo.Delete(); }
		static void __stdcall FreeNodeInfo(NN<NodeInfo> nodeInfo) { nodeInfo.Delete(); }
		static void __stdcall FreeNodeSess(NN<NodeSession> nodeSess) { nodeSess.Delete(); }
		static void __stdcall FreeAreaSess(NN<AreaSession> areaSess) { areaSess->nodes.FreeAll(FreeNodeSess); areaSess.Delete(); }
		static void AddAreaLines(NN<Data::ArrayListNN<LineInfo>> lines, NN<AreaInfo> areaInfo);
		NN<AreaInfo> GetArea(Math::Coord2DDbl pos);
		Optional<AreaInfo> GetAreaOpt(Math::Coord2DDbl pos) const;
		NN<AreaSession> GetAreaSess(NN<PathSession> sess, Math::Coord2DDbl pos) const;
		Optional<AreaInfo> GetExistingArea(IntOS areaX, IntOS areaY) const;
		NN<NodeInfo> GetNode(Math::Coord2DDbl pos, Double z);
		NN<NodeInfo> GetNodeOrUnknown(Math::Coord2DDbl pos, Double z) const;
		NN<NodeSession> GetNodeSess(NN<PathSession> sess, Math::Coord2DDbl pos, Double z) const;
		void FillNetwork(NN<NodeInfo> nodeInfo, UInt32 networkId);
	public:
		ShortestPath3D(NN<Math::CoordinateSystem> csys, Double searchDist);
		ShortestPath3D(NN<Map::MapDrawLayer> layer, Double searchDist);
		~ShortestPath3D();

		void AddSimpleLayer(NN<Map::MapDrawLayer> layer);
		Optional<LineInfo> AddPath(NN<Math::Geometry::Vector2D> path, Data::DataArray<Optional<Text::String>> properties, Bool allowReverse, Bool addToNode);
		NN<NodeInfo> AddNode(Math::Coord2DDbl pos, Double z, NN<LineInfo> lineInfo);
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
		void ClearData(NN<Math::CoordinateSystem> csys, Double searchDist);

		static void CalcDirReverse(NN<Data::ArrayListNN<Math::Geometry::LineString>> lineList, NN<Data::ArrayListNative<Double>> dirList, NN<Data::ArrayListNative<Bool>> reverseList);
	};
}
#endif
