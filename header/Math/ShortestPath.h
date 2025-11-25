#ifndef _SM_MATH_SHORTESTPATH
#define _SM_MATH_SHORTESTPATH
#include "Data/ArrayListNN.hpp"
#include "Data/Comparable.h"

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

		class Path : public Data::Comparable
		{
		public:
			double totalDistance;
			Data::ArrayListNN<PathNode> nodes;
			Optional<PathNode> lastNode;

			Path();
			virtual ~Path();
			NN<Path> Clone() const;
			Bool AddNode(NN<PathNode> node, Double distance);
			virtual OSInt CompareTo(NN<Data::Comparable> obj) const;
		};

	public:
		ShortestPath(){};
		virtual ~ShortestPath(){};

		virtual Double CalNodeDistance(NN<PathNode> node1, NN<PathNode> node2) const = 0;
		virtual Bool PathValid(Optional<PathNode> lastNode, NN<PathNode> currNode, NN<PathNode> nextNode) const = 0;
		Optional<Path> GetShortestPath(NN<PathNode> fromNode, NN<PathNode> toNode) const;
	};
}
#endif
