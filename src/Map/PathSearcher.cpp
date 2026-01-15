#include "Stdafx.h"
#include "MyMemory.h"
#include "Map/PathSearcher.h"
#include "Math/GeometryTool.h"
#include "Math/Math_C.h"

Map::PathSearcher::PointNode::PointNode(Double x, Double y, Int32 ix, Int32 iy)
{
	this->ix = ix;
	this->iy = iy;
	this->x = x;
	this->y = y;
}

Map::PathSearcher::PointNode::~PointNode()
{
}

NN<Data::ArrayListNN<Math::ShortestPath::PathNode>> Map::PathSearcher::PointNode::GetNeighbourNodes()
{
	return this->nearNodes;
}

Optional<Map::PathSearcher::PointNode> Map::PathSearcher::GetNode(Double x, Double y, OptOut<IntOS> index)
{
	IntOS i = 0;
	IntOS j = this->nodes.GetCount() - 1;
	IntOS k;
	NN<PointNode> n;
	Int32 ix = (Int32)(x * 200000.0);
	Int32 iy = (Int32)(y * 200000.0);
	while (i <= j)
	{
		k = (i + j) >> 1;
		n = this->nodes.GetItemNoCheck((UIntOS)k);
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
			index.Set(i);
			return n;
		}
	}
	index.Set(i);
	return 0;
}

NN<Map::PathSearcher::PointNode> Map::PathSearcher::GetOrAddNode(Double x, Double y)
{
	IntOS i;
	NN<Map::PathSearcher::PointNode> n;
	if (GetNode(x, y, i).SetTo(n))
	{
		return n;
	}
	NEW_CLASSNN(n, Map::PathSearcher::PointNode(x, y, (Int32)(x * 200000.0), (Int32)(y * 200000.0)));
	nodes.Insert(i, n);
	return n;
}

Map::PathSearcher::PathSearcher(NN<Map::MapDrawLayer> layer, Double minAngleRad)
{
	this->nodes = 0;
	this->minAngleRad = minAngleRad;
	if (layer->GetLayerType() != Map::DRAW_LAYER_POLYLINE)
	{
		return;
	}

	Data::ArrayListInt64 objIds;
	Optional<Map::NameArray> nameArr;
	NN<Math::Geometry::Polyline> pl;
	NN<Map::GetObjectSess> sess;
	UIntOS i;
	UIntOS j;
	UIntOS k;
	UIntOS l;
	Optional<PointNode> lastNode;
	NN<PointNode> nnlastNode;
	NN<PointNode> currNode;
	NN<Math::Geometry::LineString> ls;

	sess = layer->BeginGetObject();
	layer->GetAllObjectIds(objIds, nameArr);

	i = objIds.GetCount();
	while (i-- > 0)
	{
		if (Optional<Math::Geometry::Polyline>::ConvertFrom(layer->GetNewVectorById(sess, objIds.GetItem(i))).SetTo(pl))
		{
			UnsafeArray<Math::Coord2DDbl> points;
			j = pl->GetCount();
			while (j-- > 0)
			{
				if (pl->GetItem(j).SetTo(ls))
				{
					lastNode = 0;
					points = ls->GetPointList(k);
					while (k-- > 0)
					{
						currNode = GetOrAddNode(points[k].x, points[k].y);
						if (lastNode.SetTo(nnlastNode))
						{
							nnlastNode->nearNodes.Add(currNode);
							currNode->nearNodes.Add(nnlastNode);
						}
						lastNode = currNode;
					}
				}

			}
			pl.Delete();
		}
	}

	layer->EndGetObject(sess);
}

Map::PathSearcher::~PathSearcher()
{
	this->nodes.DeleteAll();
}

Double Map::PathSearcher::CalNodeDistance(NN<Math::ShortestPath::PathNode> node1, NN<Math::ShortestPath::PathNode> node2)
{
	NN<Map::PathSearcher::PointNode> n1 = NN<Map::PathSearcher::PointNode>::ConvertFrom(node1);
	NN<Map::PathSearcher::PointNode> n2 = NN<Map::PathSearcher::PointNode>::ConvertFrom(node2);
	Double xDiff = n1->x - n2->x;
	Double yDiff = n1->y - n2->y;
	return Math_Sqrt(xDiff * xDiff + yDiff * yDiff); //Math::Geometry::SphereDistDeg(n1->y, n1->x, n2->y, n2->x, Math::Geometry::RADIUS_METER_EARTH_WGS1984);
}

Bool Map::PathSearcher::PathValid(Optional<PathNode> lastNode, NN<PathNode> currNode, NN<PathNode> nextNode)
{
	NN<PathNode> nnlastNode;
	if (!lastNode.SetTo(nnlastNode))
		return true;
	if (lastNode == nextNode)
		return false;
	NN<PointNode> lastPoint = NN<PointNode>::ConvertFrom(nnlastNode);
	NN<PointNode> currPoint = NN<PointNode>::ConvertFrom(currNode);
	NN<PointNode> nextPoint = NN<PointNode>::ConvertFrom(nextNode);
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

Optional<Math::ShortestPath::PathNode> Map::PathSearcher::GetNearestNode(Double x, Double y)
{
	Double minDistance;
	NN<PointNode> minNode;
	Double distance;
	NN<PointNode> node;

	UIntOS i = this->nodes.GetCount();
	if (i <= 0)
		return 0;
	minNode = this->nodes.GetItemNoCheck(0);
	minDistance = Math::GeometryTool::SphereDistDeg(y, x, minNode->y, minNode->x, Math::GeometryTool::RADIUS_METER_EARTH_WGS1984);

	while (i-- > 0)
	{
		node = this->nodes.GetItemNoCheck(i);
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
	return this->nodes.GetCount() == 0;
}

NN<Math::Geometry::LineString> Map::PathSearcher::ToPolyline(NN<Math::ShortestPath::Path> path)
{
	NN<Math::Geometry::LineString> pl;
	NN<Map::PathSearcher::PointNode> n;

	UnsafeArray<Math::Coord2DDbl> points;
	UIntOS i;
	NEW_CLASSNN(pl, Math::Geometry::LineString(0, path->nodes.GetCount(), false, false));
	points = pl->GetPointList(i);

	i = 0;
	UIntOS j = path->nodes.GetCount();
	while (i < j)
	{
		n = path->nodes.GetItemNoCheck(i);
		points[i].x = n->x;
		points[i].y = n->y;
		i++;
	}
	return pl;
}
