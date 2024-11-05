#include "Stdafx.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "DB/TableDef.h"
#include "Map/ShortestPath3D.h"
#include "Math/GeometryTool.h"
#include "Math/CoordinateSystemConverter.h"
#include "Sync/MutexUsage.h"

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
	UOSInt i = lineInfo->properties.GetCount();
	while (i-- > 0)
	{
		OPTSTR_DEL(lineInfo->properties[i]);
	}
	MemFreeArr(lineInfo->properties.Arr());
	lineInfo.Delete();
}

void Map::ShortestPath3D::FreeAreaInfo(NN<AreaInfo> areaInfo)
{
	areaInfo->nodes.FreeAll(FreeNodeInfo);
	areaInfo.Delete();
}

void Map::ShortestPath3D::FreeNodeInfo(NN<NodeInfo> nodeInfo)
{
	nodeInfo.Delete();
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

Optional<Map::ShortestPath3D::AreaInfo> Map::ShortestPath3D::GetExistingArea(OSInt areaX, OSInt areaY) const
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

void Map::ShortestPath3D::AddVector(NN<Math::Geometry::Vector2D> vec, Data::DataArray<Optional<Text::String>> properties)
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
				this->AddVector(ls->Clone(), properties);
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
		i = 0;
		j = properties.GetCount();
		lineInfo->properties = Data::DataArray<Optional<Text::String>>::Alloc(j);
		while (i < j)
		{
			lineInfo->properties[i] = Text::String::CopyOrNull(properties[i]);
			i++;
		}
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
	this->lastStartHalfLine1 = 0;
	this->lastStartHalfLine2 = 0;
	this->lastEndHalfLine1 = 0;
	this->lastEndHalfLine2 = 0;
	this->propDef = 0;
	this->networkCnt = 0;
}

Map::ShortestPath3D::ShortestPath3D(NN<Map::MapDrawLayer> layer, Double searchDist)
{
	this->csys = layer->GetCoordinateSystem()->Clone();
	this->searchDist = searchDist;
	this->lastStartHalfLine1 = 0;
	this->lastStartHalfLine2 = 0;
	this->lastEndHalfLine1 = 0;
	this->lastEndHalfLine2 = 0;
	this->propDef = 0;
	this->networkCnt = 0;
	this->AddLayer(layer);
	this->BuildNetwork();
}

Map::ShortestPath3D::~ShortestPath3D()
{
	this->csys.Delete();
	this->propDef.Delete();
	this->areas.FreeAll(FreeAreaInfo);
	this->lines.FreeAll(FreeLineInfo);
	this->lastStartHalfLine1.Delete();
	this->lastStartHalfLine2.Delete();
	this->lastEndHalfLine1.Delete();
	this->lastEndHalfLine2.Delete();
}

void Map::ShortestPath3D::AddLayer(NN<Map::MapDrawLayer> layer)
{
	NN<Math::CoordinateSystem> csys = layer->GetCoordinateSystem();
	Optional<Map::NameArray> nameArr;
	Data::ArrayListInt64 idArr;
	NN<Math::Geometry::Vector2D> vec;
	if (this->propDef.IsNull())
	{
		this->propDef = layer->GetTableDef(CSTR_NULL, layer->GetSourceNameObj()->ToCString());
	}
	NN<Map::GetObjectSess> sess = layer->BeginGetObject();
	layer->GetAllObjectIds(idArr, nameArr);
	UOSInt colCnt = layer->GetColumnCnt();
	UOSInt k;
	UnsafeArray<Optional<Text::String>> properties = MemAllocArr(Optional<Text::String>, colCnt);
	Text::StringBuilderUTF8 sb;
	UOSInt i = 0;
	UOSInt j = idArr.GetCount();
	while (i < j)
	{
		if (layer->GetNewVectorById(sess, idArr.GetItem(i)).SetTo(vec))
		{
			k = colCnt;
			while (k-- > 0)
			{
				sb.ClearStr();
				if (layer->GetString(sb, nameArr, idArr.GetItem(i), k))
				{
					properties[k] = Text::String::New(sb.ToCString());
				}
				else
				{
					properties[k] = 0;
				}

			}
			if (csys->Equals(this->csys))
			{
				this->AddVector(vec, Data::DataArray<Optional<Text::String>>(properties, colCnt));
			}
			else
			{
				Math::CoordinateSystemConverter converter(csys, this->csys);
				vec->Convert(converter);
				this->AddVector(vec, Data::DataArray<Optional<Text::String>>(properties, colCnt));
			}
			k = colCnt;
			while (k-- > 0)
			{
				OPTSTR_DEL(properties[k]);
			}
		}
		i++;
	}
	layer->EndGetObject(sess);
	MemFreeArr(properties);
}

void Map::ShortestPath3D::BuildNetwork()
{
	UInt32 networkId = this->networkCnt;
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
	this->networkCnt = networkId;
}

void Map::ShortestPath3D::GetNetworkLines(NN<Data::ArrayListNN<Math::Geometry::LineString>> lines, UInt32 networkId) const
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

void Map::ShortestPath3D::GetNearestPaths(NN<Data::ArrayListNN<PathResult>> paths, Math::Coord2DDbl pos) const
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

Bool Map::ShortestPath3D::GetShortestPathDetail(Math::Coord2DDbl posStart, Math::Coord2DDbl posEnd, NN<Data::ArrayListNN<Math::Geometry::LineString>> lineList, NN<Data::ArrayListT<Data::DataArray<Optional<Text::String>>>> propList)
{
	this->lastStartHalfLine1.Delete();
	this->lastStartHalfLine2.Delete();
	this->lastEndHalfLine1.Delete();
	this->lastEndHalfLine2.Delete();
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
		return false;
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
			return false;
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
		return false;
	}
	Sync::MutexUsage mutUsage(this->mut);
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
	Double lastDist;
	this->lastStartHalfLine1 = startHalfLine1;
	if (!startHalfLine1->SplitByPoint(posStart).SetTo(startHalfLine2))
	{
		Math::Coord2DDbl nodePt;
		Double z;
		startHalfLine1->GetNearEnd(posStart, nodePt, z);
		startHalfLine1.Delete();
		this->lastStartHalfLine2 = 0;
		NEW_CLASSNN(startHalfLine1, Math::Geometry::LineString(this->csys->GetSRID(), 2, true, false));
		this->lastStartHalfLine1 = startHalfLine1;
		UOSInt nPoint;
		UnsafeArray<Math::Coord2DDbl> pointList = startHalfLine1->GetPointList(nPoint);
		UnsafeArray<Double> zList;
		pointList[0] = nodePt;
		pointList[1] = nodePt;
		if (startHalfLine1->GetZList(nPoint).SetTo(zList))
		{
			zList[0] = z;
			zList[1] = z;
		}
		nodeInfo = GetNode(nodePt, z);
		nodeInfo->calcNodeDist = 0;
		nodeInfo->calcFrom = posStart;
		nodeInfo->calcFromZ = 0;
		nodeInfo->calcLine = startHalfLine1;
		nodeInfo->calcLineProp = path1->line->properties;
		calcNodes.Add(nodeInfo);
	}
	else
	{
		this->lastStartHalfLine2 = startHalfLine2;
		///////////////////////////
		// if (path1 == path2)
		startHalfLine1->Reverse();
		nodeInfo = GetNode(path1->line->startPos, path1->line->startZ);
		nodeInfo->calcNodeDist = lastDist = startHalfLine1->Calc3DLength();
		nodeInfo->calcFrom = posStart;
		nodeInfo->calcFromZ = 0;
		nodeInfo->calcLine = startHalfLine1;
		nodeInfo->calcLineProp = path1->line->properties;
		calcNodes.Add(nodeInfo);
		nodeInfo = GetNode(path1->line->endPos, path1->line->endZ);
		nodeInfo->calcNodeDist = startHalfLine2->Calc3DLength();
		nodeInfo->calcFrom = posStart;
		nodeInfo->calcFromZ = 0;
		nodeInfo->calcLine = startHalfLine2;
		nodeInfo->calcLineProp = path1->line->properties;
		if (nodeInfo->calcNodeDist < lastDist)
		{
			calcNodes.Insert(0, nodeInfo);
		}
		else
		{
			calcNodes.Add(nodeInfo);
		}
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
				destNodeInfo->calcLineProp = lineInfo->properties;
			}
			else if (destNodeInfo->calcNodeDist > lastDist)
			{
				destNodeInfo->calcNodeDist = lastDist;
				destNodeInfo->calcFrom = nodeInfo->pos;
				destNodeInfo->calcFromZ = nodeInfo->z;
				destNodeInfo->calcLine = lineInfo->vec;
				destNodeInfo->calcLineProp = lineInfo->properties;
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
		this->lastStartHalfLine1.Delete();
		this->lastStartHalfLine2.Delete();
		paths1.DeleteAll();
		paths2.DeleteAll();
		printf("GetShortestPath start node or end node not found\r\n");
		return false;
	}
	NN<Math::Geometry::LineString> endHalfLine1 = NN<Math::Geometry::LineString>::ConvertFrom(path2->vec->Clone());
	NN<Math::Geometry::LineString> endHalfLine2;
	if (!endHalfLine1->SplitByPoint(posEnd).SetTo(endHalfLine2))
	{
		endHalfLine1.Delete();
		this->lastEndHalfLine1 = 0;
		this->lastEndHalfLine2 = 0;
		nodeInfo = GetNode(path2->line->endPos, path2->line->endZ);
	}
	else
	{
		endHalfLine2->Reverse();
		lastDist = endHalfLine1->Calc3DLength();
		nodeInfo = GetNode(path2->line->startPos, path2->line->startZ);
		lastDist += nodeInfo->calcNodeDist;
		nodeInfo = GetNode(path2->line->endPos, path2->line->endZ);
		if (nodeInfo->calcNodeDist + endHalfLine2->Calc3DLength() < lastDist)
		{
			lineList->Add(endHalfLine2);
			propList->Add(path2->line->properties);
		}
		else
		{
			nodeInfo = GetNode(path2->line->startPos, path2->line->startZ);
			lineList->Add(endHalfLine1);
			propList->Add(path2->line->properties);
		}
		this->lastEndHalfLine1 = endHalfLine1;
		this->lastEndHalfLine2 = endHalfLine2;
	}
	while (true)
	{
		if (nodeInfo->calcFrom == posStart && nodeInfo->calcFromZ == 0)
		{
			if (nodeInfo->calcNodeDist > 0)
			{
				lineList->Add(nodeInfo->calcLine);
				propList->Add(nodeInfo->calcLineProp);
			}
			break;
		}
		lineList->Add(nodeInfo->calcLine);
		propList->Add(nodeInfo->calcLineProp);
		nodeInfo = GetNode(nodeInfo->calcFrom, nodeInfo->calcFromZ);
	}
	lineList->Reverse();
	propList->Reverse();
	paths1.DeleteAll();
	paths2.DeleteAll();
	return true;
}

Optional<Math::Geometry::LineString> Map::ShortestPath3D::GetShortestPath(Math::Coord2DDbl posStart, Math::Coord2DDbl posEnd)
{
	Data::ArrayListNN<Math::Geometry::LineString> lineList;
	Data::ArrayListT<Data::DataArray<Optional<Text::String>>> propList;
	if (!GetShortestPathDetail(posStart, posEnd, lineList, propList))
		return 0;
	return Math::Geometry::LineString::JoinLines(lineList);
}

NN<Math::CoordinateSystem> Map::ShortestPath3D::GetCoordinateSystem() const
{
	return this->csys;
}

Optional<DB::TableDef> Map::ShortestPath3D::GetPropDef() const
{
	return this->propDef;
}

UInt32 Map::ShortestPath3D::GetNetworkCnt() const
{
	return this->networkCnt;
}

void Map::ShortestPath3D::CalcDirReverse(NN<Data::ArrayListNN<Math::Geometry::LineString>> lineList, NN<Data::ArrayList<Double>> dirList, NN<Data::ArrayList<Bool>> reverseList)
{
	NN<Math::Geometry::LineString> ls;
	NN<Math::Geometry::LineString> nextLS;
	UnsafeArray<const Math::Coord2DDbl> thisPoints;
	UnsafeArray<const Math::Coord2DDbl> nextPoints;
	Math::Coord2DDbl thisEndPt;
	Bool isReverse = false;
	Double dir1;
	Double dir2;
	UOSInt k;
	UOSInt l;
	UOSInt i = 0;
	UOSInt j = lineList->GetCount();
	if (j >= 2)
	{
		ls = lineList->GetItemNoCheck(0);
		nextLS = lineList->GetItemNoCheck(1);
		thisPoints = ls->GetPointListRead(k);
		nextPoints = nextLS->GetPointListRead(l);
		if (thisPoints[0] == nextPoints[0] || thisPoints[0] == nextPoints[l - 1])
		{
			isReverse = true;
		}
	}
	while (i < j)
	{
		ls = lineList->GetItemNoCheck(i);
		reverseList->Add(isReverse);
		if (i + 1 < j)
		{
			nextLS = lineList->GetItemNoCheck(i + 1);
			thisPoints = ls->GetPointListRead(k);
			nextPoints = nextLS->GetPointListRead(l);
			if (isReverse)
			{
				dir1 = Math::GeometryTool::CalcDir(thisPoints[1], thisPoints[0], Math::Unit::Angle::AU_DEGREE);
				thisEndPt = thisPoints[0];
			}
			else
			{
				dir1 = Math::GeometryTool::CalcDir(thisPoints[k - 2], thisPoints[k - 1], Math::Unit::Angle::AU_DEGREE);
				thisEndPt = thisPoints[k - 1];
			}
			if (thisEndPt == nextPoints[l - 1])
			{
				dir2 = Math::GeometryTool::CalcDir(nextPoints[l - 1], nextPoints[l - 2], Math::Unit::Angle::AU_DEGREE);
				isReverse = true;
			}
			else
			{
				dir2 = Math::GeometryTool::CalcDir(nextPoints[0], nextPoints[1], Math::Unit::Angle::AU_DEGREE);
				isReverse = false;
			}
			Double dir = dir1 - dir2;
			if (dir < 0)
			{
				dir += 360;
			}
			dirList->Add(dir);
		}
		i++;
	}
}
