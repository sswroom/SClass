#ifndef _SM_MATH_SHORTESTPATH
#define _SM_MATH_SHORTESTPATH
#include "Data/ArrayListNN.h"
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
			virtual NN<Data::ArrayListNN<PathNode>> GetNeighbourNodes() = 0;
		};

		class Path : public Data::IComparable
		{
		public:
			double totalDistance;
			Data::ArrayListNN<PathNode> nodes;
			Optional<PathNode> lastNode;

			Path();
			virtual ~Path();
			NN<Path> Clone() const;
			Bool AddNode(NN<PathNode> node, Double distance);
			virtual OSInt CompareTo(Data::IComparable *obj) const;
		};

	public:
		ShortestPath(){};
		virtual ~ShortestPath(){};

		virtual Double CalNodeDistance(PathNode *node1, PathNode *node2) const = 0;
		virtual Bool PathValid(PathNode *lastNode, PathNode *currNode, PathNode *nextNode const) = 0;
		Optional<Path> GetShortestPath(NN<PathNode> fromNode, NN<PathNode> toNode) const;
	};
}
#endif
