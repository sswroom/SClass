#include "Stdafx.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "Map/ShortestPath3D.h"
#include "Math/CoordinateSystemConverter.h"

#define MAX_DIST 1.0e+30

Map::ShortestPath3D::LineComparator::LineComparator()
{
}

Map::ShortestPath3D::LineComparator::~LineComparator()
{

}

OSInt Map::ShortestPath3D::LineComparator::Compare(NN<LineInfo> a, NN<LineInfo> b) const
{
	if (a->index > b->index)
	{
		return 1;
	}
	else if (a->index < b->index)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

Map::ShortestPath3D::PathResultComparator::PathResultComparator()
{
}

Map::ShortestPath3D::PathResultComparator::~PathResultComparator()
{
}

OSInt Map::ShortestPath3D::PathResultComparator::Compare(NN<PathResult> a, NN<PathResult> b) const
{
	if (a->dist > b->dist)
	{
		return 1;
	}
	else if (a->dist < b->dist)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

Map::ShortestPath3D::NodeDistanceComparator::NodeDistanceComparator()
{
}

Map::ShortestPath3D::NodeDistanceComparator::~NodeDistanceComparator()
{
}

OSInt Map::ShortestPath3D::NodeDistanceComparator::Compare(NN<NodeInfo> a, NN<NodeInfo> b) const
{
	if (a->calcNodeDist > b->calcNodeDist)
	{
		return 1;
	}
	else if (a->calcNodeDist < b->calcNodeDist)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

void Map::ShortestPath3D::FreeLineInfo(NN<LineInfo> lineInfo)
{
	lineInfo->vec.Delete();
	lineInfo.Delete();
}

void Map::ShortestPath3D::FreeAreaInfo(NN<AreaInfo> areaInfo)
{
	areaInfo->nodes.DeleteAll();
	areaInfo.Delete();
}

void Map::ShortestPath3D::AddAreaLines(NN<Data::ArrayListNN<LineInfo>> lines, NN<AreaInfo> areaInfo)
{
	UOSInt i = 0;
	UOSInt j = areaInfo->nodes.GetCount();
	NN<NodeInfo> nodeInfo;
	while (i < j)
	{
		nodeInfo = areaInfo->nodes.GetItemNoCheck(i);
		lines->AddAll(nodeInfo->lines);
		i++;
	}
}

NN<Map::ShortestPath3D::AreaInfo> Map::ShortestPath3D::GetArea(Math::Coord2DDbl pos)
{
	OSInt areaX = (OSInt)(pos.x / this->searchDist);
	OSInt areaY = (OSInt)(pos.y / this->searchDist);
	OSInt i = 0;
	OSInt j = (OSInt)this->areas.GetCount() - 1;
	OSInt k;
	NN<AreaInfo> areaInfo;
	while (i <= j)
	{
		k = (i + j) >> 1;
		areaInfo = this->areas.GetItemNoCheck((UOSInt)k);
		if (areaInfo->x > areaX)
		{
			j = k - 1;
		}
		else if (areaInfo->x < areaX)
		{
			i = k + 1;
		}
		else if (areaInfo->y > areaY)
		{
			j = k - 1;
		}
		else if (areaInfo->y < areaY)
		{
			i = k + 1;
		}
		else
		{
			return areaInfo;
		}
	}
	NEW_CLASSNN(areaInfo, AreaInfo());
	areaInfo->x = areaX;
	areaInfo->y = areaY;
	this->areas.Insert((UOSInt)i, areaInfo);
	return areaInfo;
}

Optional<Map::ShortestPath3D::AreaInfo> Map::ShortestPath3D::GetExistingArea(OSInt areaX, OSInt areaY)
{
	OSInt i = 0;
	OSInt j = (OSInt)this->areas.GetCount() - 1;
	OSInt k;
	NN<AreaInfo> areaInfo;
	while (i <= j)
	{
		k = (i + j) >> 1;
		areaInfo = this->areas.GetItemNoCheck((UOSInt)k);
		if (areaInfo->x > areaX)
		{
			j = k - 1;
		}
		else if (areaInfo->x < areaX)
		{
			i = k + 1;
		}
		else if (areaInfo->y > areaY)
		{
			j = k - 1;
		}
		else if (areaInfo->y < areaY)
		{
			i = k + 1;
		}
		else
		{
			return areaInfo;
		}
	}
	return 0;
}

NN<Map::ShortestPath3D::NodeInfo> Map::ShortestPath3D::GetNode(Math::Coord2DDbl pos, Double z)
{
	NN<AreaInfo> areaInfo = GetArea(pos);
	OSInt i = 0;
	OSInt j = (OSInt)areaInfo->nodes.GetCount() - 1;
	OSInt k;
	NN<NodeInfo> nodeInfo;
	while (i <= j)
	{
		k = (i + j) >> 1;
		nodeInfo = areaInfo->nodes.GetItemNoCheck((UOSInt)k);
		if (nodeInfo->pos.x > pos.x)
		{
			j = k - 1;
		}
		else if (nodeInfo->pos.x < pos.x)
		{
			i = k + 1;
		}
		else if (nodeInfo->pos.y > pos.y)
		{
			j = k - 1;
		}
		else if (nodeInfo->pos.y < pos.y)
		{
			i = k + 1;
		}
		else if (nodeInfo->z > z)
		{
			j = k - 1;
		}
		else if (nodeInfo->z < z)
		{
			i = k + 1;
		}
		else
		{
			return nodeInfo;
		}
	}
	NEW_CLASSNN(nodeInfo, NodeInfo());
	nodeInfo->pos = pos;
	nodeInfo->z = z;
	nodeInfo->networkId = 0;
	areaInfo->nodes.Insert((UOSInt)i, nodeInfo);
	return nodeInfo;
}

void Map::ShortestPath3D::FillNetwork(NN<NodeInfo> nodeInfo, UInt32 networkId)
{
	Data::ArrayListNN<NodeInfo> nodes;
	nodes.Add(nodeInfo);
	while (nodes.Pop().SetTo(nodeInfo))
	{
		UOSInt i = 0;
		UOSInt j = nodeInfo->lines.GetCount();
		NN<LineInfo> lineInfo;
		NN<NodeInfo> nextNode;
		nodeInfo->networkId = networkId;
		while (i < j)
		{
			lineInfo = nodeInfo->lines.GetItemNoCheck(i);
			lineInfo->networkId = networkId;
			i++;
		}
		i = 0;
		while (i < j)
		{
			lineInfo = nodeInfo->lines.GetItemNoCheck(i);
			if (lineInfo->startPos == nodeInfo->pos && lineInfo->startZ == nodeInfo->z)
			{
				nextNode = this->GetNode(lineInfo->endPos, lineInfo->endZ);
			}
			else
			{
				nextNode = this->GetNode(lineInfo->startPos, lineInfo->startZ);
			}
			if (nextNode->networkId == 0)
			{
				nodes.Add(nextNode);
			}
			i++;
		}
	}
}

void Map::ShortestPath3D::AddVector(NN<Math::Geometry::Vector2D> vec)
{
	Math::Geometry::Vector2D::VectorType type = vec->GetVectorType();
	NN<Math::Geometry::Polyline> pl;
	NN<Math::Geometry::LineString> ls;
	NN<LineInfo> lineInfo;
	NN<NodeInfo> nodeInfo;
	UnsafeArray<Math::Coord2DDbl> ptList;
	UnsafeArray<Double> zList;
	UOSInt nPoints;
	UOSInt i;
	UOSInt j;
	switch (type)
	{
	case Math::Geometry::Vector2D::VectorType::Polyline:
		pl = NN<Math::Geometry::Polyline>::ConvertFrom(vec);
		i = 0;
		j = pl->GetCount();
		while (i < j)
		{
			if (pl->GetItem(i).SetTo(ls))
			{
				this->AddVector(ls->Clone());
			}
			i++;
		}
		vec.Delete();
		break;
	case Math::Geometry::Vector2D::VectorType::LineString:
		ls = NN<Math::Geometry::LineString>::ConvertFrom(vec);
		NEW_CLASSNN(lineInfo, LineInfo());
		ptList = ls->GetPointList(nPoints);
		lineInfo->startPos = ptList[0];
		lineInfo->endPos = ptList[nPoints - 1];
		if (ls->GetZList(nPoints).SetTo(zList))
		{
			lineInfo->startZ = zList[0];
			lineInfo->endZ = zList[nPoints - 1];
		}
		else
		{
			lineInfo->startZ = 0;
			lineInfo->endZ = 0;
		}
		lineInfo->index = this->lines.GetCount();
		lineInfo->networkId = 0;
		lineInfo->vec = ls;
		lineInfo->length = ls->Calc3DLength();
		this->lines.Add(lineInfo);
		nodeInfo = this->GetNode(lineInfo->startPos, lineInfo->startZ);
		nodeInfo->lines.Add(lineInfo);
		nodeInfo = this->GetNode(lineInfo->endPos, lineInfo->endZ);
		nodeInfo->lines.Add(lineInfo);
		break;
	case Math::Geometry::Vector2D::VectorType::Unknown:
	case Math::Geometry::Vector2D::VectorType::Point:
	case Math::Geometry::Vector2D::VectorType::Polygon:
	case Math::Geometry::Vector2D::VectorType::MultiPoint:
	case Math::Geometry::Vector2D::VectorType::MultiPolygon:
	case Math::Geometry::Vector2D::VectorType::GeometryCollection:
	case Math::Geometry::Vector2D::VectorType::CircularString:
	case Math::Geometry::Vector2D::VectorType::CompoundCurve:
	case Math::Geometry::Vector2D::VectorType::CurvePolygon:
	case Math::Geometry::Vector2D::VectorType::MultiCurve:
	case Math::Geometry::Vector2D::VectorType::MultiSurface:
	case Math::Geometry::Vector2D::VectorType::Curve:
	case Math::Geometry::Vector2D::VectorType::Surface:
	case Math::Geometry::Vector2D::VectorType::PolyhedralSurface:
	case Math::Geometry::Vector2D::VectorType::Tin:
	case Math::Geometry::Vector2D::VectorType::Triangle:
	case Math::Geometry::Vector2D::VectorType::LinearRing:
	case Math::Geometry::Vector2D::VectorType::Image:
	case Math::Geometry::Vector2D::VectorType::String:
	case Math::Geometry::Vector2D::VectorType::Ellipse:
	case Math::Geometry::Vector2D::VectorType::PieArea:
	default:
		printf("Unsupported vector type: %s\r\n", Math::Geometry::Vector2D::VectorTypeGetName(type).v.Ptr());
		vec.Delete();
		break;
	}
}

Map::ShortestPath3D::ShortestPath3D(NN<Math::CoordinateSystem> csys, Double searchDist)
{
	this->csys = csys;
	this->searchDist = searchDist;
}

Map::ShortestPath3D::~ShortestPath3D()
{
	this->csys.Delete();
	this->areas.FreeAll(FreeAreaInfo);
	this->lines.FreeAll(FreeLineInfo);
}

void Map::ShortestPath3D::AddLayer(NN<Map::MapDrawLayer> layer)
{
	NN<Math::CoordinateSystem> csys = layer->GetCoordinateSystem();
	Optional<Map::NameArray> nameArr;
	Data::ArrayListInt64 idArr;
	NN<Math::Geometry::Vector2D> vec;
	NN<Map::GetObjectSess> sess = layer->BeginGetObject();
	layer->GetAllObjectIds(idArr, nameArr);
	UOSInt i = 0;
	UOSInt j = idArr.GetCount();
	while (i < j)
	{
		if (layer->GetNewVectorById(sess, idArr.GetItem(i)).SetTo(vec))
		{
			if (csys->Equals(this->csys))
			{
				this->AddVector(vec);
			}
			else
			{
				Math::CoordinateSystemConverter converter(csys, this->csys);
				vec->Convert(converter);
				this->AddVector(vec);
			}
		}
		i++;
	}
	layer->EndGetObject(sess);
}

void Map::ShortestPath3D::BuildNetwork()
{
	UInt32 networkId = 0;
	NN<AreaInfo> areaInfo;
	NN<NodeInfo> nodeInfo;
	UOSInt i = 0;
	UOSInt j = this->areas.GetCount();
	UOSInt k;
	UOSInt l;
	while (i < j)
	{
		areaInfo = this->areas.GetItemNoCheck(i);
		k = 0;
		l = areaInfo->nodes.GetCount();
		while (k < l)
		{
			nodeInfo = areaInfo->nodes.GetItemNoCheck(k);
			if (nodeInfo->networkId == 0)
			{
				networkId++;
				this->FillNetwork(nodeInfo, networkId);
			}
			k++;
		}
		i++;
	}
}

void Map::ShortestPath3D::GetNetworkLines(NN<Data::ArrayListNN<Math::Geometry::LineString>> lines, UInt32 networkId)
{
	NN<LineInfo> lineInfo;
	UOSInt i = 0;
	UOSInt j = this->lines.GetCount();
	while (i < j)
	{
		lineInfo = this->lines.GetItemNoCheck(i);
		if (lineInfo->networkId == networkId)
		{
			lines->Add(lineInfo->vec);
		}
		i++;
	}
}

void Map::ShortestPath3D::GetNearestPaths(NN<Data::ArrayListNN<PathResult>> paths, Math::Coord2DDbl pos)
{
	OSInt x = (OSInt)(pos.x / this->searchDist);
	OSInt y = (OSInt)(pos.y / this->searchDist);
	Data::ArrayListNN<LineInfo> lines;
	NN<AreaInfo> areaInfo;
	if (GetExistingArea(x, y).SetTo(areaInfo))
		AddAreaLines(lines, areaInfo);
	if (GetExistingArea(x - 1, y).SetTo(areaInfo))
		AddAreaLines(lines, areaInfo);
	if (GetExistingArea(x + 1, y).SetTo(areaInfo))
		AddAreaLines(lines, areaInfo);
	if (GetExistingArea(x, y - 1).SetTo(areaInfo))
		AddAreaLines(lines, areaInfo);
	if (GetExistingArea(x, y + 1).SetTo(areaInfo))
		AddAreaLines(lines, areaInfo);
	LineComparator comparator;
	Data::Sort::ArtificialQuickSort::Sort<NN<LineInfo>>(lines, comparator);
	NN<PathResult> res;
	Math::Coord2DDbl nearestPt;
	Double dist;
	NN<LineInfo> lineInfo;
	UOSInt lastIndex = INVALID_INDEX;
	UOSInt i = 0;
	UOSInt j = lines.GetCount();
	while (i < j)
	{
		lineInfo = lines.GetItemNoCheck(i);
		if (lineInfo->index != lastIndex)
		{
			lastIndex = lineInfo->index;
			dist = lineInfo->vec->CalSqrDistance(pos, nearestPt);
			NEW_CLASSNN(res, PathResult());
			res->dist = Math_Sqrt(dist);
			res->point = nearestPt;
			res->line = lineInfo;
			res->vec = lineInfo->vec;
			paths->Add(res);
		}
		i++;
	}
	PathResultComparator pcomparator;
	Data::Sort::ArtificialQuickSort::Sort<NN<PathResult>>(paths, pcomparator);
}

Optional<Math::Geometry::LineString> Map::ShortestPath3D::GetShortestPath(Math::Coord2DDbl posStart, Math::Coord2DDbl posEnd)
{
	Data::ArrayListNN<PathResult> paths1;
	Data::ArrayListNN<PathResult> paths2;
	NN<PathResult> path1;
	NN<PathResult> path2;
	GetNearestPaths(paths1, posStart);
	GetNearestPaths(paths2, posEnd);
	UOSInt i1;
	UOSInt i2;
	UOSInt j1;
	UOSInt j2;
	i1 = 0;
	i2 = 0;
	j1 = paths1.GetCount();
	j2 = paths2.GetCount();
	if (j1 == 0 || j2 == 0)
		return 0;
	path1 = paths1.GetItemNoCheck(0);
	path2 = paths2.GetItemNoCheck(0);
	if (path1->line->networkId != path2->line->networkId)
	{
		while (i2 < j2)
		{
			path2 = paths2.GetItemNoCheck(i2);
			if (path1->line->networkId == path2->line->networkId)
				break;
			i2++;
		}
		path2 = paths2.GetItemNoCheck(0);
		while (i1 < j1)
		{
			path1 = paths1.GetItemNoCheck(i1);
			if (path1->line->networkId == path2->line->networkId)
				break;
			i1++;
		}
		if (i1 >= j1 && i2 >= j2)
		{
			paths1.DeleteAll();
			paths2.DeleteAll();
			return 0;
		}
		if (i1 >= j1)
		{
			i1 = 0;
		}
		else if (i2 >= j2)
		{
			i2 = 0;
		}
		else if (i1 > j1)
		{
			i1 = 0;
		}
		else
		{
			i2 = 0;
		}
		path1 = paths1.GetItemNoCheck(i1);
		path2 = paths2.GetItemNoCheck(i2);
	}
	if (path1->line->networkId != path2->line->networkId)
	{
		paths1.DeleteAll();
		paths2.DeleteAll();
		printf("GetShortestPath internal error\r\n");
		return 0;
	}

	Data::ArrayListNN<NodeInfo> calcNodes;
	NN<AreaInfo> areaInfo;
	NN<NodeInfo> nodeInfo;
	NN<NodeInfo> destNodeInfo;
	NN<LineInfo> lineInfo;
	i1 = 0;
	j1 = this->areas.GetCount();
	while (i1 < j1)
	{
		areaInfo = this->areas.GetItemNoCheck(i1);
		i2 = 0;
		j2 = areaInfo->nodes.GetCount();
		while (i2 < j2)
		{
			nodeInfo = areaInfo->nodes.GetItemNoCheck(i2);
			if (nodeInfo->networkId == path1->line->networkId)
			{
				nodeInfo->calcNodeDist = MAX_DIST;
			}
			i2++;
		}
		i1++;
	}
	NN<Math::Geometry::LineString> startHalfLine1 = NN<Math::Geometry::LineString>::ConvertFrom(path1->vec->Clone());
	NN<Math::Geometry::LineString> startHalfLine2;
	if (!startHalfLine1->SplitByPoint(posStart).SetTo(startHalfLine2))
	{
		startHalfLine1.Delete();
		paths1.DeleteAll();
		paths2.DeleteAll();
		printf("GetShortestPath split start error\r\n");
		return 0;
	}
	startHalfLine1->Reverse();
	Double lastDist;
	nodeInfo = GetNode(path1->line->startPos, path1->line->startZ);
	nodeInfo->calcNodeDist = lastDist = startHalfLine1->Calc3DLength();
	nodeInfo->calcFrom = posStart;
	nodeInfo->calcFromZ = 0;
	nodeInfo->calcLine = startHalfLine1;
	calcNodes.Add(nodeInfo);
	nodeInfo = GetNode(path1->line->endPos, path1->line->endZ);
	nodeInfo->calcNodeDist = startHalfLine2->Calc3DLength();
	nodeInfo->calcFrom = posStart;
	nodeInfo->calcFromZ = 0;
	nodeInfo->calcLine = startHalfLine2;
	if (nodeInfo->calcNodeDist < lastDist)
	{
		calcNodes.Insert(0, nodeInfo);
	}
	else
	{
		calcNodes.Add(nodeInfo);
	}
	Bool startFound = false;
	Bool endFound = false;
	NodeDistanceComparator comparator;
	i1 = 0;
	while (i1 < calcNodes.GetCount())
	{
		nodeInfo = calcNodes.GetItemNoCheck(i1);
		i2 = 0;
		j2 = nodeInfo->lines.GetCount();
		while (i2 < j2)
		{
			lineInfo = nodeInfo->lines.GetItemNoCheck(i2);
			lastDist = nodeInfo->calcNodeDist + lineInfo->length;
			if (lineInfo->startPos == nodeInfo->pos && lineInfo->startZ == nodeInfo->z)
			{
				destNodeInfo = GetNode(lineInfo->endPos, lineInfo->endZ);
			}
			else
			{
				destNodeInfo = GetNode(lineInfo->startPos, lineInfo->startZ);
			}
			if (destNodeInfo->calcNodeDist == MAX_DIST)
			{
				calcNodes.Add(destNodeInfo);
				destNodeInfo->calcNodeDist = lastDist;
				destNodeInfo->calcFrom = nodeInfo->pos;
				destNodeInfo->calcFromZ = nodeInfo->z;
				destNodeInfo->calcLine = lineInfo->vec;
			}
			else if (destNodeInfo->calcNodeDist > lastDist)
			{
				destNodeInfo->calcNodeDist = lastDist;
				destNodeInfo->calcFrom = nodeInfo->pos;
				destNodeInfo->calcFromZ = nodeInfo->z;
				destNodeInfo->calcLine = lineInfo->vec;
			}
			i2++;
		}
		i1++;
		Data::Sort::ArtificialQuickSort::Sort<NN<NodeInfo>>(calcNodes.Arr(), comparator, (OSInt)i1, (OSInt)calcNodes.GetCount() - 1);
		if (nodeInfo->pos == path2->line->startPos && nodeInfo->z == path2->line->startZ)
		{
			startFound = true;
			if (endFound)
				break;
		}
		else if (nodeInfo->pos == path2->line->endPos && nodeInfo->z == path2->line->endZ)
		{
			endFound = true;
			if (startFound)
				break;
		}
	}

	if (!startFound || !endFound)
	{
		startHalfLine1.Delete();
		startHalfLine2.Delete();
		paths1.DeleteAll();
		paths2.DeleteAll();
		printf("GetShortestPath start node or end node not found\r\n");
		return 0;
	}
	NN<Math::Geometry::LineString> endHalfLine1 = NN<Math::Geometry::LineString>::ConvertFrom(path2->vec->Clone());
	NN<Math::Geometry::LineString> endHalfLine2;
	if (!endHalfLine1->SplitByPoint(posEnd).SetTo(endHalfLine2))
	{
		startHalfLine1.Delete();
		startHalfLine2.Delete();
		endHalfLine1.Delete();
		paths1.DeleteAll();
		paths2.DeleteAll();
		printf("GetShortestPath split end error\r\n");
		return 0;
	}
	Math::Coord2DDbl linePt;
	Data::ArrayListNN<Math::Geometry::LineString> lineList;
	endHalfLine2->Reverse();
	lastDist = endHalfLine1->Calc3DLength();
	nodeInfo = GetNode(path2->line->startPos, path2->line->startZ);
	lastDist += nodeInfo->calcNodeDist;
	nodeInfo = GetNode(path2->line->endPos, path2->line->endZ);
	if (nodeInfo->calcNodeDist + endHalfLine2->Calc3DLength() < lastDist)
	{
		linePt = endHalfLine2->GetPoint(0);
		lineList.Add(endHalfLine2);
	}
	else
	{
		nodeInfo = GetNode(path2->line->startPos, path2->line->startZ);
		linePt = endHalfLine1->GetPoint(0);
		lineList.Add(endHalfLine1);
	}
	while (true)
	{
		linePt = nodeInfo->calcLine->GetPoint(0);
		lineList.Add(nodeInfo->calcLine);
		if (nodeInfo->calcFrom == posStart && nodeInfo->calcFromZ == 0)
			break;
		nodeInfo = GetNode(nodeInfo->calcFrom, nodeInfo->calcFromZ);
	}
	lineList.Reverse();
	Optional<Math::Geometry::LineString> ret = Math::Geometry::LineString::JoinLines(lineList);
	startHalfLine1.Delete();
	startHalfLine2.Delete();
	endHalfLine1.Delete();
	endHalfLine2.Delete();
	paths1.DeleteAll();
	paths2.DeleteAll();
	return ret;
}

NN<Math::CoordinateSystem> Map::ShortestPath3D::GetCoordinateSystem() const
{
	return this->csys;
}
