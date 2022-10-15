#ifndef _SM_MAP_PATHSEARCHER
#define _SM_MAP_PATHSEARCHER
#include "Map/IMapDrawLayer.h"
#include "Math/ShortestPath.h"
#include "Math/Geometry/Polyline.h"

namespace Map
{
	class PathSearcher : public Math::ShortestPath
	{
	private:
		class PointNode : public Math::ShortestPath::PathNode
		{
		public:
			Double x;
			Double y;
			Int32 ix;
			Int32 iy;
			Data::ArrayList<Math::ShortestPath::PathNode*> *nearNodes;

			PointNode(Double x, Double y, Int32 ix, Int32 iy);
			virtual ~PointNode();
			virtual Data::ArrayList<Math::ShortestPath::PathNode*> *GetNeighbourNodes();
		};
	private:
		Data::ArrayList<Math::ShortestPath::PathNode*> *nodes;
		Double minAngleRad;

	private:
		Map::PathSearcher::PointNode *GetNode(Double x, Double y, Bool toAdd);
	public:
		PathSearcher(Map::IMapDrawLayer *layer, Double  minAngleRad);
		virtual ~PathSearcher();
		
		virtual Double CalNodeDistance(PathNode *node1, PathNode *node2);
		virtual Bool PathValid(PathNode *lastNode, PathNode *currNode, PathNode *nextNode);
		Math::ShortestPath::PathNode *GetNearestNode(Double x, Double y);
		Bool IsError();
		static Math::Geometry::Polyline *ToPolyline(Math::ShortestPath::Path *path);
	};
};
#endif
