#include "Stdafx.h"
#include "MyMemory.h"
#include "Map/PathSearcher.h"
#include "Math/GeometryTool.h"
#include "Math/Math.h"

Map::PathSearcher::PointNode::PointNode(Double x, Double y, Int32 ix, Int32 iy)
{
	this->ix = ix;
	this->iy = iy;
	this->x = x;
	this->y = y;
	NEW_CLASS(this->nearNodes, Data::ArrayList<Math::ShortestPath::PathNode*>());
}

Map::PathSearcher::PointNode::~PointNode()
{
	DEL_CLASS(this->nearNodes);
}

Data::ArrayList<Math::ShortestPath::PathNode*> *Map::PathSearcher::PointNode::GetNeighbourNodes()
{
	return this->nearNodes;
}

Map::PathSearcher::PointNode *Map::PathSearcher::GetNode(Double x, Double y, Bool toAdd)
{
	OSInt i = 0;
	OSInt j = nodes->GetCount() - 1;
	OSInt k;
	PointNode *n;
	Int32 ix = (Int32)(x * 200000.0);
	Int32 iy = (Int32)(y * 200000.0);
/*	if (ix == 22827576 && iy == 4457438)
	{
		ix = 22827576;
		iy = 4457438;
	}*/
	while (i <= j)
	{
		k = (i + j) >> 1;
		n = (PointNode*)nodes->GetItem(k);
		if (n->ix > ix)
		{
			j = k - 1;
		}
		else if (n->ix < ix)
		{
			i = k + 1;
		}
		else if (n->iy > iy)
		{
			j = k - 1;
		}
		else if (n->iy < iy)
		{
			i = k + 1;
		}
		else
		{
			return n;
		}
	}
	if (toAdd)
	{
		NEW_CLASS(n, Map::PathSearcher::PointNode(x, y, ix, iy));
		nodes->Insert(i, n);
		return n;
	}
	else
	{
		return 0;
	}
}

Map::PathSearcher::PathSearcher(Map::MapDrawLayer *layer, Double minAngleRad)
{
	this->nodes = 0;
	this->minAngleRad = minAngleRad;
	if (layer->GetLayerType() != Map::DRAW_LAYER_POLYLINE)
	{
		return;
	}

	Data::ArrayListInt64 *objIds;
	void *nameArr;
	Math::Geometry::Polyline *pl;
	void *sess;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	PointNode *lastNode;
	PointNode *currNode;

	NEW_CLASS(nodes, Data::ArrayList<Math::ShortestPath::PathNode*>());
	NEW_CLASS(objIds, Data::ArrayListInt64());
	sess = layer->BeginGetObject();
	layer->GetAllObjectIds(objIds, &nameArr);

	i = objIds->GetCount();
	while (i-- > 0)
	{
		pl = (Math::Geometry::Polyline*)layer->GetNewVectorById(sess, objIds->GetItem(i));
		if (pl)
		{
			Math::Coord2DDbl *points;
			UInt32 *parts;
			points = pl->GetPointList(&k);
			parts = pl->GetPtOfstList(&j);
			while (j-- > 0)
			{
				lastNode = 0;
				l = parts[j];
				while (k-- > l)
				{
					currNode = GetNode(points[k].x, points[k].y, true);
					if (lastNode)
					{
						lastNode->nearNodes->Add(currNode);
						currNode->nearNodes->Add(lastNode);
					}
					lastNode = currNode;
				}
				k = l;
			}
			DEL_CLASS(pl);
		}
	}

	DEL_CLASS(objIds);
	layer->EndGetObject(sess);
}

Map::PathSearcher::~PathSearcher()
{
	if (nodes)
	{
		PointNode *n;
		OSInt i = nodes->GetCount();
		while (i-- > 0)
		{
			n = (PointNode*)nodes->GetItem(i);
			DEL_CLASS(n);
		}
		DEL_CLASS(nodes);
		nodes = 0;
	}
}

Double Map::PathSearcher::CalNodeDistance(Math::ShortestPath::PathNode *node1, Math::ShortestPath::PathNode *node2)
{
	Map::PathSearcher::PointNode *n1 = (Map::PathSearcher::PointNode*)node1;
	Map::PathSearcher::PointNode *n2 = (Map::PathSearcher::PointNode*)node2;
	Double xDiff = n1->x - n2->x;
	Double yDiff = n1->y - n2->y;
	return Math_Sqrt(xDiff * xDiff + yDiff * yDiff); //Math::Geometry::SphereDistDeg(n1->y, n1->x, n2->y, n2->x, Math::Geometry::RADIUS_METER_EARTH_WGS1984);
}

Bool Map::PathSearcher::PathValid(PathNode *lastNode, PathNode *currNode, PathNode *nextNode)
{
	if (lastNode == 0)
		return true;
	if (lastNode == nextNode)
		return false;
	PointNode *lastPoint = (PointNode*)lastNode;
	PointNode *currPoint = (PointNode*)currNode;
	PointNode *nextPoint = (PointNode*)nextNode;
	Double angle1;
	Double angle2;
	if (lastPoint->x == currPoint->x && lastPoint->y == currPoint->y)
		return true;
	if (nextPoint->x == currPoint->x && nextPoint->y == currPoint->y)
		return true;
	angle1 = Math_ArcTan2(lastPoint->y - currPoint->y, lastPoint->x - currPoint->x);
	angle2 = Math_ArcTan2(currPoint->y - nextPoint->y, currPoint->x - nextPoint->x);
	angle1 = angle1 - angle2;
	if (angle1 < 0)
	{
		angle1 = -angle1;
	}
	if (angle1 > Math::PI)
	{
		angle1 = Math::PI * 2 - angle1;
	}
	else if (angle1 < -Math::PI)
	{
		angle1 = Math::PI * 2 + angle1;
	}
	if (angle1 >= minAngleRad)
	{
		return false;
	}
	return angle1 < minAngleRad;
}

Math::ShortestPath::PathNode *Map::PathSearcher::GetNearestNode(Double x, Double y)
{
	Double minDistance;
	PointNode *minNode;
	Double distance;
	PointNode *node;

	OSInt i = this->nodes->GetCount();
	if (i <= 0)
		return 0;
	minNode = (Map::PathSearcher::PointNode*)this->nodes->GetItem(0);
	minDistance = Math::GeometryTool::SphereDistDeg(y, x, minNode->y, minNode->x, Math::GeometryTool::RADIUS_METER_EARTH_WGS1984);

	while (i-- > 0)
	{
		node = (Map::PathSearcher::PointNode*)this->nodes->GetItem(i);
		distance = Math::GeometryTool::SphereDistDeg(y, x, node->y, node->x, Math::GeometryTool::RADIUS_METER_EARTH_WGS1984);
		if (distance < minDistance)
		{
			minDistance = distance;
			minNode = node;
		}
	}
	return minNode;
}

Bool Map::PathSearcher::IsError()
{
	return this->nodes == 0;
}

Math::Geometry::Polyline *Map::PathSearcher::ToPolyline(Math::ShortestPath::Path *path)
{
	Math::Geometry::Polyline *pl;
	Map::PathSearcher::PointNode *n;

	if (path == 0)
		return 0;

	Math::Coord2DDbl *points;
	UOSInt i;
	NEW_CLASS(pl, Math::Geometry::Polyline(0, 1, path->nodes->GetCount(), false, false));
	pl->GetPtOfstList(&i)[0] = 0;
	points = pl->GetPointList(&i);

	i = 0;
	OSInt j = path->nodes->GetCount();
	while (i < j)
	{
		n = (Map::PathSearcher::PointNode*)path->nodes->GetItem(i);
		points[i].x = n->x;
		points[i].y = n->y;
		i++;
	}
	return pl;
}
