#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/ShortestPath.h"
#include "Data/ArrayListCmp.h"

Math::ShortestPath::Path::Path()
{
	this->totalDistance = 0;
	this->lastNode = 0;
}

Math::ShortestPath::Path::~Path()
{
}

NN<Math::ShortestPath::Path> Math::ShortestPath::Path::Clone() const
{
	NN<Math::ShortestPath::Path> newPath;
	NEW_CLASSNN(newPath, Math::ShortestPath::Path());
	newPath->nodes.AddAll(this->nodes);
	newPath->totalDistance = totalDistance;
	newPath->lastNode = lastNode;
	return newPath;
}

Bool Math::ShortestPath::Path::AddNode(NN<Math::ShortestPath::PathNode> node, Double distance)
{
	UOSInt i = this->nodes.GetCount();
	while (i-- > 0)
	{
		if (this->nodes.GetItem(i) == node)
			return false;
	}
	this->nodes.Add(node);
	this->lastNode = node;
	totalDistance += distance;
	return true;
}

OSInt Math::ShortestPath::Path::CompareTo(Data::IComparable *obj) const
{
	Math::ShortestPath::Path *path2 = (Math::ShortestPath::Path*)obj;
	if (this->totalDistance > path2->totalDistance)
	{
		return -1;
	}
	else if (this->totalDistance < path2->totalDistance)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

Optional<Math::ShortestPath::Path> Math::ShortestPath::GetShortestPath(NN<Math::ShortestPath::PathNode> fromNode, NN<Math::ShortestPath::PathNode> toNode) const
{
	NN<Math::ShortestPath::Path> path;
	NN<Math::ShortestPath::Path> tmpPath;
	Optional<Math::ShortestPath::PathNode> currNode;
	Optional<Math::ShortestPath::PathNode> nextNode;
	Data::ArrayList<Math::ShortestPath::PathNode*> *neighbourNodes;
	Data::ArrayListNN<Path> paths;
	Double distance;
	OSInt cnt;
	OSInt i;

	NEW_CLASSNN(path, Math::ShortestPath::Path());
	path->AddNode(fromNode, 0);
	paths.Add(path);

	while (true)
	{
		cnt = paths.GetCount();
		path = paths.GetItemNoCheck(cnt - 1);
		paths.RemoveAt(cnt - 1);
		if (path->lastNode == toNode)
		{
			cnt--;
			while (cnt-- > 0)
			{
				tmpPath = paths.GetItemNoCheck(cnt);
				tmpPath.Delete();
			}
			return path;
		}

		tmpPath = path->Clone();
		currNode = path->lastNode;
		neighbourNodes = currNode->GetNeighbourNodes();
		i = neighbourNodes->GetCount();
		while (i-- > 0)
		{
			nextNode = neighbourNodes->GetItem(i);
			if (this->PathValid(path->nodes.GetItemNoCheck(path->nodes.GetCount() - 2), currNode, nextNode))
			{
				distance = this->CalNodeDistance(currNode, nextNode);
				if (distance >= 0)
				{
					if (tmpPath->AddNode(nextNode, distance))
					{
						paths->SortedInsert(tmpPath);
						tmpPath = path->Clone();
						if (paths.GetCount() >= 10000)
							break;
					}
				}
			}
		}
		path.Delete();
		tmpPath.Delete();

		if (paths.GetCount() == 0)
		{
			return 0;
		}
	}
	return 0;
}
