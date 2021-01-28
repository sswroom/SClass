#ifndef _SM_MATH_SHORTESTPATH
#define _SM_MATH_SHORTESTPATH
#include "Data/ArrayList.h"
#include "Data/IComparable.h"

namespace Math
{
	class ShortestPath
	{
	public:
		class PathNode
		{
		public:
			virtual ~PathNode(){};
			virtual Data::ArrayList<PathNode*> *GetNeighbourNodes() = 0;
		};

		class Path : public Data::IComparable
		{
		public:
			double totalDistance;
			Data::ArrayList<PathNode*> *nodes;
			PathNode *lastNode;

			Path();
			virtual ~Path();
			Path *Clone();
			Bool AddNode(PathNode *node, Double distance);
			virtual Int32 CompareTo(Data::IComparable *obj);
		};

	public:
		ShortestPath(){};
		virtual ~ShortestPath(){};

		virtual Double CalNodeDistance(PathNode *node1, PathNode *node2) = 0;
		virtual Bool PathValid(PathNode *lastNode, PathNode *currNode, PathNode *nextNode) = 0;
		Path *GetShortestPath(PathNode *fromNode, PathNode *toNode);
	};
}
#endif
