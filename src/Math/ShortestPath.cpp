#include "stdafx.h"
#include "MyMemory.h"
#include "Math/ShortestPath.h"
#include "Data/ArrayListCmp.h"

Math::ShortestPath::Path::Path()
{
	totalDistance = 0;
	NEW_CLASS(nodes, Data::ArrayList<Math::ShortestPath::PathNode*>());
}

Math::ShortestPath::Path::~Path()
{
	DEL_CLASS(nodes);
}

Math::ShortestPath::Path *Math::ShortestPath::Path::Clone()
{
	Math::ShortestPath::Path *newPath;
	NEW_CLASS(newPath, Math::ShortestPath::Path());
	newPath->nodes->AddRange(nodes);
	newPath->totalDistance = totalDistance;
	newPath->lastNode = lastNode;
	return newPath;
}

Bool Math::ShortestPath::Path::AddNode(Math::ShortestPath::PathNode *node, Double distance)
{
	OSInt i = nodes->GetCount();
	while (i-- > 0)
	{
		if (nodes->GetItem(i) == node)
			return false;
	}
	nodes->Add(this->lastNode = node);
	totalDistance += distance;
	return true;
}

Int32 Math::ShortestPath::Path::CompareTo(Data::IComparable *obj)
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

Math::ShortestPath::Path *Math::ShortestPath::GetShortestPath(Math::ShortestPath::PathNode *fromNode, Math::ShortestPath::PathNode *toNode)
{
	Math::ShortestPath::Path *path;
	Math::ShortestPath::Path *tmpPath;
	Math::ShortestPath::PathNode *currNode;
	Math::ShortestPath::PathNode *nextNode;
	Data::ArrayList<Math::ShortestPath::PathNode*> *neighbourNodes;
	Data::ArrayListCmp *paths;
	Double distance;
	OSInt cnt;
	OSInt i;

	NEW_CLASS(paths, Data::ArrayListCmp());
	NEW_CLASS(path, Math::ShortestPath::Path());
	path->AddNode(fromNode, 0);
	paths->Add(path);

	while (true)
	{
		cnt = paths->GetCount();
		path = (Math::ShortestPath::Path*)paths->RemoveAt(cnt - 1);
		if (path->lastNode == toNode)
		{
			cnt--;
			while (cnt-- > 0)
			{
				tmpPath = (Math::ShortestPath::Path*)paths->GetItem(cnt);
				DEL_CLASS(tmpPath);
			}
			DEL_CLASS(paths);
			return path;
		}

		tmpPath = path->Clone();
		currNode = path->lastNode;
		neighbourNodes = currNode->GetNeighbourNodes();
		i = neighbourNodes->GetCount();
		while (i-- > 0)
		{
			nextNode = (Math::ShortestPath::PathNode*)neighbourNodes->GetItem(i);
			if (this->PathValid(path->nodes->GetItem(path->nodes->GetCount() - 2), currNode, nextNode))
			{
				distance = this->CalNodeDistance(currNode, nextNode);
				if (distance >= 0)
				{
					if (tmpPath->AddNode(nextNode, distance))
					{
						paths->SortedInsert(tmpPath);
						tmpPath = path->Clone();
						if (paths->GetCount() >= 10000)
							break;
					}
				}
			}
		}
		DEL_CLASS(path);
		DEL_CLASS(tmpPath);

		if (paths->GetCount() == 0)
		{
			DEL_CLASS(paths);
			return 0;
		}
	}
	return 0;
}
