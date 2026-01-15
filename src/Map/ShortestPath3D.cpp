#include "Stdafx.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "DB/TableDef.h"
#include "Map/ShortestPath3D.h"
#include "Math/CoordinateSystemConverter.h"
#include "Math/GeometryTool.h"
#include "Math/Geometry/MultiCurve.h"
#include "Sync/MutexUsage.h"

#define MAX_DIST 1.0e+30

Map::ShortestPath3D::LineComparator::LineComparator()
{
}

Map::ShortestPath3D::LineComparator::~LineComparator()
{

}

IntOS Map::ShortestPath3D::LineComparator::Compare(NN<LineInfo> a, NN<LineInfo> b) const
{
	return (IntOS)a->index - (IntOS)b->index;
}

Map::ShortestPath3D::PathResultComparator::PathResultComparator()
{
}

Map::ShortestPath3D::PathResultComparator::~PathResultComparator()
{
}

IntOS Map::ShortestPath3D::PathResultComparator::Compare(NN<PathResult> a, NN<PathResult> b) const
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

IntOS Map::ShortestPath3D::NodeDistanceComparator::Compare(NN<NodeSession> a, NN<NodeSession> b) const
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
	UIntOS i = lineInfo->properties.GetCount();
	while (i-- > 0)
	{
		OPTSTR_DEL(lineInfo->properties[i]);
	}
	MemFreeArr(lineInfo->properties.Arr());
	lineInfo.Delete();
}

void Map::ShortestPath3D::AddAreaLines(NN<Data::ArrayListNN<LineInfo>> lines, NN<AreaInfo> areaInfo)
{
	UIntOS i = 0;
	UIntOS j = areaInfo->nodes.GetCount();
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
	IntOS areaX = (IntOS)(pos.x / this->searchDist);
	IntOS areaY = (IntOS)(pos.y / this->searchDist);
	IntOS i = 0;
	IntOS j = (IntOS)this->areas.GetCount() - 1;
	IntOS k;
	NN<AreaInfo> areaInfo;
	while (i <= j)
	{
		k = (i + j) >> 1;
		areaInfo = this->areas.GetItemNoCheck((UIntOS)k);
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
	this->areas.Insert((UIntOS)i, areaInfo);
	return areaInfo;
}

Optional<Map::ShortestPath3D::AreaInfo> Map::ShortestPath3D::GetAreaOpt(Math::Coord2DDbl pos) const
{
	IntOS areaX = (IntOS)(pos.x / this->searchDist);
	IntOS areaY = (IntOS)(pos.y / this->searchDist);
	IntOS i = 0;
	IntOS j = (IntOS)this->areas.GetCount() - 1;
	IntOS k;
	NN<AreaInfo> areaInfo;
	while (i <= j)
	{
		k = (i + j) >> 1;
		areaInfo = this->areas.GetItemNoCheck((UIntOS)k);
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
	return nullptr;
}

NN<Map::ShortestPath3D::AreaSession> Map::ShortestPath3D::GetAreaSess(NN<PathSession> sess, Math::Coord2DDbl pos) const
{
	IntOS areaX = (IntOS)(pos.x / this->searchDist);
	IntOS areaY = (IntOS)(pos.y / this->searchDist);
	IntOS i = 0;
	IntOS j = (IntOS)sess->areas.GetCount() - 1;
	IntOS k;
	NN<AreaSession> areaSess;
	while (i <= j)
	{
		k = (i + j) >> 1;
		areaSess = sess->areas.GetItemNoCheck((UIntOS)k);
		if (areaSess->x > areaX)
		{
			j = k - 1;
		}
		else if (areaSess->x < areaX)
		{
			i = k + 1;
		}
		else if (areaSess->y > areaY)
		{
			j = k - 1;
		}
		else if (areaSess->y < areaY)
		{
			i = k + 1;
		}
		else
		{
			return areaSess;
		}
	}
	NEW_CLASSNN(areaSess, AreaSession());
	areaSess->x = areaX;
	areaSess->y = areaY;
	sess->areas.Insert((UIntOS)i, areaSess);
	return areaSess;
}

Optional<Map::ShortestPath3D::AreaInfo> Map::ShortestPath3D::GetExistingArea(IntOS areaX, IntOS areaY) const
{
	IntOS i = 0;
	IntOS j = (IntOS)this->areas.GetCount() - 1;
	IntOS k;
	NN<AreaInfo> areaInfo;
	while (i <= j)
	{
		k = (i + j) >> 1;
		areaInfo = this->areas.GetItemNoCheck((UIntOS)k);
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
	return nullptr;
}

NN<Map::ShortestPath3D::NodeInfo> Map::ShortestPath3D::GetNode(Math::Coord2DDbl pos, Double z)
{
	NN<AreaInfo> areaInfo = GetArea(pos);
	IntOS i = 0;
	IntOS j = (IntOS)areaInfo->nodes.GetCount() - 1;
	IntOS k;
	NN<NodeInfo> nodeInfo;
	while (i <= j)
	{
		k = (i + j) >> 1;
		nodeInfo = areaInfo->nodes.GetItemNoCheck((UIntOS)k);
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
	areaInfo->nodes.Insert((UIntOS)i, nodeInfo);
	return nodeInfo;
}

NN<Map::ShortestPath3D::NodeInfo> Map::ShortestPath3D::GetNodeOrUnknown(Math::Coord2DDbl pos, Double z) const
{
	NN<AreaInfo> areaInfo;
	if (!GetAreaOpt(pos).SetTo(areaInfo))
	{
		return this->unknownNode;
	}
	IntOS i = 0;
	IntOS j = (IntOS)areaInfo->nodes.GetCount() - 1;
	IntOS k;
	NN<NodeInfo> nodeInfo;
	while (i <= j)
	{
		k = (i + j) >> 1;
		nodeInfo = areaInfo->nodes.GetItemNoCheck((UIntOS)k);
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
	return this->unknownNode;
}

NN<Map::ShortestPath3D::NodeSession> Map::ShortestPath3D::GetNodeSess(NN<PathSession> sess, Math::Coord2DDbl pos, Double z) const
{
	NN<AreaSession> areaSess = GetAreaSess(sess, pos);
	IntOS i = 0;
	IntOS j = (IntOS)areaSess->nodes.GetCount() - 1;
	IntOS k;
	NN<NodeSession> nodeSess;
	NN<NodeInfo> nodeInfo;
	while (i <= j)
	{
		k = (i + j) >> 1;
		nodeSess = areaSess->nodes.GetItemNoCheck((UIntOS)k);
		nodeInfo = nodeSess->node;
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
			return nodeSess;
		}
	}
	NEW_CLASSNN(nodeSess, NodeSession());
	nodeSess->node = GetNodeOrUnknown(pos, z);
	nodeSess->calcNodeDist = MAX_DIST;
	areaSess->nodes.Insert((UIntOS)i, nodeSess);
	return nodeSess;
}

void Map::ShortestPath3D::FillNetwork(NN<NodeInfo> nodeInfo, UInt32 networkId)
{
	Data::ArrayListNN<NodeInfo> nodes;
	nodes.Add(nodeInfo);
	while (nodes.Pop().SetTo(nodeInfo))
	{
		UIntOS i = 0;
		UIntOS j = nodeInfo->lines.GetCount();
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

Map::ShortestPath3D::ShortestPath3D(NN<Math::CoordinateSystem> csys, Double searchDist)
{
	this->csys = csys;
	this->searchDist = searchDist;
	this->propDef = nullptr;
	this->networkCnt = 0;
	NEW_CLASSNN(this->unknownNode, NodeInfo());
	this->unknownNode->pos = Math::Coord2DDbl(0, 0);
	this->unknownNode->z = 0;
	this->unknownNode->networkId = 0;
}

Map::ShortestPath3D::ShortestPath3D(NN<Map::MapDrawLayer> layer, Double searchDist)
{
	this->csys = layer->GetCoordinateSystem()->Clone();
	this->searchDist = searchDist;
	this->propDef = nullptr;
	this->networkCnt = 0;
	NEW_CLASSNN(this->unknownNode, NodeInfo());
	this->unknownNode->pos = Math::Coord2DDbl(0, 0);
	this->unknownNode->z = 0;
	this->unknownNode->networkId = 0;
	this->AddSimpleLayer(layer);
	this->BuildNetwork();
}

Map::ShortestPath3D::~ShortestPath3D()
{
	this->csys.Delete();
	this->propDef.Delete();
	this->areas.FreeAll(FreeAreaInfo);
	this->lines.FreeAll(FreeLineInfo);
	this->unknownNode.Delete();
}

void Map::ShortestPath3D::AddSimpleLayer(NN<Map::MapDrawLayer> layer)
{
	NN<Math::CoordinateSystem> csys = layer->GetCoordinateSystem();
	Optional<Map::NameArray> nameArr;
	Data::ArrayListInt64 idArr;
	NN<Math::Geometry::Vector2D> vec;
	if (this->propDef.IsNull())
	{
		this->propDef = layer->GetTableDef(nullptr, layer->GetSourceNameObj()->ToCString());
	}
	NN<Map::GetObjectSess> sess = layer->BeginGetObject();
	layer->GetAllObjectIds(idArr, nameArr);
	UIntOS colCnt = layer->GetColumnCnt();
	UIntOS k;
	UnsafeArray<Optional<Text::String>> properties = MemAllocArr(Optional<Text::String>, colCnt);
	Text::StringBuilderUTF8 sb;
	UIntOS i = 0;
	UIntOS j = idArr.GetCount();
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
					properties[k] = nullptr;
				}

			}
			if (csys->Equals(this->csys))
			{
				this->AddPath(vec, Data::DataArray<Optional<Text::String>>(properties, colCnt), true, true);
			}
			else
			{
				Math::CoordinateSystemConverter converter(csys, this->csys);
				vec->Convert(converter);
				this->AddPath(vec, Data::DataArray<Optional<Text::String>>(properties, colCnt), true, true);
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
	layer->ReleaseNameArr(nameArr);
	MemFreeArr(properties);
}

Optional<Map::ShortestPath3D::LineInfo> Map::ShortestPath3D::AddPath(NN<Math::Geometry::Vector2D> vec, Data::DataArray<Optional<Text::String>> properties, Bool allowReverse, Bool addToNode)
{
	Optional<LineInfo> retInfo = nullptr;
	Math::Geometry::Vector2D::VectorType type = vec->GetVectorType();
	NN<Math::Geometry::Polyline> pl;
	NN<Math::Geometry::LineString> ls;
	NN<LineInfo> lineInfo;
	NN<NodeInfo> nodeInfo;
	UnsafeArray<Math::Coord2DDbl> ptList;
	UnsafeArray<Double> zList;
	UIntOS nPoints;
	UIntOS i;
	UIntOS j;
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
				retInfo = this->AddPath(ls->Clone(), properties, allowReverse, addToNode);
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
		lineInfo->rect = ls->GetBounds();
		lineInfo->allowReverse = allowReverse;
		i = 0;
		j = properties.GetCount();
		lineInfo->properties = Data::DataArray<Optional<Text::String>>::Alloc(j);
		while (i < j)
		{
			lineInfo->properties[i] = Text::String::CopyOrNull(properties[i]);
			i++;
		}
		lineInfo->length = ls->Calc3DLength();
		retInfo = lineInfo;
		this->lines.Add(lineInfo);
		if (addToNode)
		{
			nodeInfo = this->GetNode(lineInfo->startPos, lineInfo->startZ);
			nodeInfo->lines.Add(lineInfo);
			nodeInfo = this->GetNode(lineInfo->endPos, lineInfo->endZ);
			nodeInfo->lines.Add(lineInfo);
		}
		break;
	case Math::Geometry::Vector2D::VectorType::MultiCurve:
		{
			NN<Math::Geometry::Vector2D> vec2;
			if (NN<Math::Geometry::MultiCurve>::ConvertFrom(vec)->ToSimpleShape().SetTo(vec2))
			{
				retInfo = this->AddPath(vec2, properties, allowReverse, addToNode);
			}
			vec.Delete();
			break;
		}
	case Math::Geometry::Vector2D::VectorType::Unknown:
	case Math::Geometry::Vector2D::VectorType::Point:
	case Math::Geometry::Vector2D::VectorType::Polygon:
	case Math::Geometry::Vector2D::VectorType::MultiPoint:
	case Math::Geometry::Vector2D::VectorType::MultiPolygon:
	case Math::Geometry::Vector2D::VectorType::GeometryCollection:
	case Math::Geometry::Vector2D::VectorType::CircularString:
	case Math::Geometry::Vector2D::VectorType::CompoundCurve:
	case Math::Geometry::Vector2D::VectorType::CurvePolygon:
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
	return retInfo;
}

NN<Map::ShortestPath3D::NodeInfo> Map::ShortestPath3D::AddNode(Math::Coord2DDbl pos, Double z, NN<LineInfo> lineInfo)
{
	Double diffZ = (lineInfo->startZ - z);
	Double startDiff = (lineInfo->startPos - pos).SumSqr() + diffZ * diffZ;
	diffZ = (lineInfo->endZ - z);
	Double endDiff = (lineInfo->endPos - pos).SumSqr() + diffZ * diffZ;
	if (startDiff < endDiff)
	{
		NN<NodeInfo> nodeInfo = this->GetNode(lineInfo->startPos, lineInfo->startZ);
		nodeInfo->lines.Add(lineInfo);
		return nodeInfo;
	}
	else
	{
		NN<NodeInfo> nodeInfo = this->GetNode(lineInfo->endPos, lineInfo->endZ);
		nodeInfo->lines.Add(lineInfo);
		return nodeInfo;
	}
}

void Map::ShortestPath3D::BuildNetwork()
{
	UInt32 networkId = this->networkCnt;
	NN<AreaInfo> areaInfo;
	NN<NodeInfo> nodeInfo;
	UIntOS i = 0;
	UIntOS j = this->areas.GetCount();
	UIntOS k;
	UIntOS l;
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
	this->lastUpdated = Data::DateTimeUtil::GetCurrTimeMillis();
}

void Map::ShortestPath3D::GetNetworkLines(NN<Data::ArrayListNN<Math::Geometry::LineString>> lines, UInt32 networkId) const
{
	NN<LineInfo> lineInfo;
	UIntOS i = 0;
	UIntOS j = this->lines.GetCount();
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
	IntOS x = (IntOS)(pos.x / this->searchDist);
	IntOS y = (IntOS)(pos.y / this->searchDist);
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
	UIntOS lastIndex = INVALID_INDEX;
	UIntOS i = 0;
	UIntOS j = lines.GetCount();
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

NN<Map::ShortestPath3D::PathSession> Map::ShortestPath3D::CreateSession() const
{
	NN<PathSession> sess;
	NEW_CLASSNN(sess, PathSession());
	sess->lastEndHalfLine1 = nullptr;
	sess->lastEndHalfLine2 = nullptr;
	sess->lastStartHalfLine1 = nullptr;
	sess->lastStartHalfLine2 = nullptr;
	return sess;
}

void Map::ShortestPath3D::FreeSession(NN<PathSession> sess) const
{
	sess->lastEndHalfLine1.Delete();
	sess->lastEndHalfLine2.Delete();
	sess->lastStartHalfLine1.Delete();
	sess->lastStartHalfLine2.Delete();
	sess->areas.FreeAll(FreeAreaSess);
	sess.Delete();
}

Bool Map::ShortestPath3D::GetShortestPathDetail(NN<PathSession> sess, Math::Coord2DDbl posStart, Math::Coord2DDbl posEnd, NN<Data::ArrayListNN<Math::Geometry::LineString>> lineList, NN<Data::ArrayListT<Data::DataArray<Optional<Text::String>>>> propList) const
{
	sess->lastStartHalfLine1.Delete();
	sess->lastStartHalfLine2.Delete();
	sess->lastEndHalfLine1.Delete();
	sess->lastEndHalfLine2.Delete();
	if (sess->lastUpdated != this->lastUpdated)
	{
		sess->lastUpdated = this->lastUpdated;
		sess->areas.FreeAll(FreeAreaSess);
	}
	Data::ArrayListNN<PathResult> paths1;
	Data::ArrayListNN<PathResult> paths2;
	NN<PathResult> path1;
	NN<PathResult> path2;
	GetNearestPaths(paths1, posStart);
	GetNearestPaths(paths2, posEnd);
	UIntOS i1;
	UIntOS i2;
	UIntOS j1;
	UIntOS j2;
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
	Data::ArrayListNN<NodeSession> calcNodes;
	NN<AreaSession> areaSess;
	NN<NodeSession> nodeSess;
	NN<NodeSession> destNodeSess;
	NN<LineInfo> lineInfo;
	i1 = 0;
	j1 = sess->areas.GetCount();
	while (i1 < j1)
	{
		areaSess = sess->areas.GetItemNoCheck(i1);
		i2 = 0;
		j2 = areaSess->nodes.GetCount();
		while (i2 < j2)
		{
			nodeSess = areaSess->nodes.GetItemNoCheck(i2);
			nodeSess->calcNodeDist = MAX_DIST;
			i2++;
		}
		i1++;
	}
	NN<Math::Geometry::LineString> startHalfLine1 = NN<Math::Geometry::LineString>::ConvertFrom(path1->vec->Clone());
	NN<Math::Geometry::LineString> startHalfLine2;
	Double lastDist;
	sess->lastStartHalfLine1 = startHalfLine1;
	if (!startHalfLine1->SplitByPoint(posStart).SetTo(startHalfLine2))
	{
		Math::Coord2DDbl nodePt;
		Double z;
		startHalfLine1->GetNearEnd(posStart, nodePt, z);
		startHalfLine1.Delete();
		sess->lastStartHalfLine2 = nullptr;
		NEW_CLASSNN(startHalfLine1, Math::Geometry::LineString(this->csys->GetSRID(), 2, true, false));
		sess->lastStartHalfLine1 = startHalfLine1;
		UIntOS nPoint;
		UnsafeArray<Math::Coord2DDbl> pointList = startHalfLine1->GetPointList(nPoint);
		UnsafeArray<Double> zList;
		pointList[0] = nodePt;
		pointList[1] = nodePt;
		if (startHalfLine1->GetZList(nPoint).SetTo(zList))
		{
			zList[0] = z;
			zList[1] = z;
		}
		nodeSess = GetNodeSess(sess, nodePt, z);
		nodeSess->calcNodeDist = 0;
		nodeSess->calcFrom = posStart;
		nodeSess->calcFromZ = 0;
		nodeSess->calcLine = startHalfLine1;
		nodeSess->calcLineProp = path1->line->properties;
		calcNodes.Add(nodeSess);
	}
	else
	{
		sess->lastStartHalfLine2 = startHalfLine2;
		if (path1->line->allowReverse)
		{
			startHalfLine1->Reverse();
			nodeSess = GetNodeSess(sess, path1->line->startPos, path1->line->startZ);
			nodeSess->calcNodeDist = lastDist = startHalfLine1->Calc3DLength();
			nodeSess->calcFrom = posStart;
			nodeSess->calcFromZ = 0;
			nodeSess->calcLine = startHalfLine1;
			nodeSess->calcLineProp = path1->line->properties;
			calcNodes.Add(nodeSess);
			nodeSess = GetNodeSess(sess, path1->line->endPos, path1->line->endZ);
			nodeSess->calcNodeDist = startHalfLine2->Calc3DLength();
			nodeSess->calcFrom = posStart;
			nodeSess->calcFromZ = 0;
			nodeSess->calcLine = startHalfLine2;
			nodeSess->calcLineProp = path1->line->properties;
			if (nodeSess->calcNodeDist < lastDist)
			{
				calcNodes.Insert(0, nodeSess);
			}
			else
			{
				calcNodes.Add(nodeSess);
			}
		}
		else
		{
			nodeSess = GetNodeSess(sess, path1->line->endPos, path1->line->endZ);
			nodeSess->calcNodeDist = startHalfLine2->Calc3DLength();
			nodeSess->calcFrom = posStart;
			nodeSess->calcFromZ = 0;
			nodeSess->calcLine = startHalfLine2;
			nodeSess->calcLineProp = path1->line->properties;
			calcNodes.Add(nodeSess);
		}
	}
	Bool startFound = false;
	Bool endFound = false;
	Optional<Map::ShortestPath3D::NodeSession> optDestNodeSess;
	NodeDistanceComparator comparator;
	i1 = 0;
	while (i1 < calcNodes.GetCount())
	{
		nodeSess = calcNodes.GetItemNoCheck(i1);
		i2 = 0;
		j2 = nodeSess->node->lines.GetCount();
		while (i2 < j2)
		{
			lineInfo = nodeSess->node->lines.GetItemNoCheck(i2);
			lastDist = nodeSess->calcNodeDist + lineInfo->length;
			if (lineInfo->startPos == nodeSess->node->pos && lineInfo->startZ == nodeSess->node->z)
			{
				optDestNodeSess = GetNodeSess(sess, lineInfo->endPos, lineInfo->endZ);
			}
			else if (lineInfo->allowReverse)
			{
				optDestNodeSess = GetNodeSess(sess, lineInfo->startPos, lineInfo->startZ);
			}
			else
			{
				optDestNodeSess = nullptr;
			}
			if (optDestNodeSess.SetTo(destNodeSess))
			{
				if (destNodeSess->calcNodeDist == MAX_DIST)
				{
					calcNodes.Add(destNodeSess);
					destNodeSess->calcNodeDist = lastDist;
					destNodeSess->calcFrom = nodeSess->node->pos;
					destNodeSess->calcFromZ = nodeSess->node->z;
					destNodeSess->calcLine = lineInfo->vec;
					destNodeSess->calcLineProp = lineInfo->properties;
				}
				else if (destNodeSess->calcNodeDist > lastDist)
				{
					destNodeSess->calcNodeDist = lastDist;
					destNodeSess->calcFrom = nodeSess->node->pos;
					destNodeSess->calcFromZ = nodeSess->node->z;
					destNodeSess->calcLine = lineInfo->vec;
					destNodeSess->calcLineProp = lineInfo->properties;
				}
			}
			i2++;
		}
		i1++;
		Data::Sort::ArtificialQuickSort::Sort<NN<NodeSession>>(calcNodes.Arr(), comparator, (IntOS)i1, (IntOS)calcNodes.GetCount() - 1);
		if (nodeSess->node->pos == path2->line->startPos && nodeSess->node->z == path2->line->startZ)
		{
			startFound = true;
			if (endFound)
				break;
		}
		if (nodeSess->node->pos == path2->line->endPos && nodeSess->node->z == path2->line->endZ)
		{
			endFound = true;
			if (startFound)
				break;
		}
	}

	if (!startFound || !endFound)
	{
		sess->lastStartHalfLine1.Delete();
		sess->lastStartHalfLine2.Delete();
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
		sess->lastEndHalfLine1 = nullptr;
		sess->lastEndHalfLine2 = nullptr;
		nodeSess = GetNodeSess(sess, path2->line->endPos, path2->line->endZ);
	}
	else
	{
		if (path2->line->allowReverse)
		{
			endHalfLine2->Reverse();
			lastDist = endHalfLine1->Calc3DLength();
			nodeSess = GetNodeSess(sess, path2->line->startPos, path2->line->startZ);
			lastDist += nodeSess->calcNodeDist;
			nodeSess = GetNodeSess(sess, path2->line->endPos, path2->line->endZ);
			if (nodeSess->calcNodeDist + endHalfLine2->Calc3DLength() < lastDist)
			{
				lineList->Add(endHalfLine2);
				propList->Add(path2->line->properties);
			}
			else
			{
				nodeSess = GetNodeSess(sess, path2->line->startPos, path2->line->startZ);
				lineList->Add(endHalfLine1);
				propList->Add(path2->line->properties);
			}
		}
		else
		{
			nodeSess = GetNodeSess(sess, path2->line->startPos, path2->line->startZ);
			lineList->Add(endHalfLine1);
			propList->Add(path2->line->properties);
		}
		sess->lastEndHalfLine1 = endHalfLine1;
		sess->lastEndHalfLine2 = endHalfLine2;
	}
	while (true)
	{
		if (nodeSess->calcFrom == posStart && nodeSess->calcFromZ == 0)
		{
			if (nodeSess->calcNodeDist > 0)
			{
				lineList->Add(nodeSess->calcLine);
				propList->Add(nodeSess->calcLineProp);
			}
			break;
		}
		lineList->Add(nodeSess->calcLine);
		propList->Add(nodeSess->calcLineProp);
		nodeSess = GetNodeSess(sess, nodeSess->calcFrom, nodeSess->calcFromZ);
	}
	lineList->Reverse();
	propList->Reverse();
	paths1.DeleteAll();
	paths2.DeleteAll();
	return true;
}

Optional<Math::Geometry::LineString> Map::ShortestPath3D::GetShortestPath(NN<PathSession> sess, Math::Coord2DDbl posStart, Math::Coord2DDbl posEnd) const
{
	Data::ArrayListNN<Math::Geometry::LineString> lineList;
	Data::ArrayListT<Data::DataArray<Optional<Text::String>>> propList;
	if (!GetShortestPathDetail(sess, posStart, posEnd, lineList, propList))
		return nullptr;
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

void Map::ShortestPath3D::GetLines(Math::RectAreaDbl rect, NN<Data::ArrayListNN<Math::Geometry::LineString>> lineList) const
{
	NN<LineInfo> lineInfo;
	UIntOS i = 0;
	UIntOS j = this->lines.GetCount();
	while (i < j)
	{
		lineInfo = this->lines.GetItemNoCheck(i);
		if (lineInfo->rect.OverlapOrTouch(rect))
		{
			lineList->Add(lineInfo->vec);
		}
		i++;
	}
}

void Map::ShortestPath3D::ClearData(NN<Math::CoordinateSystem> csys, Double searchDist)
{
	this->propDef.Delete();
	this->areas.FreeAll(FreeAreaInfo);
	this->lines.FreeAll(FreeLineInfo);
	this->networkCnt = 0;
	this->searchDist = searchDist;
	this->csys.Delete();
	this->csys = csys;
}

void Map::ShortestPath3D::CalcDirReverse(NN<Data::ArrayListNN<Math::Geometry::LineString>> lineList, NN<Data::ArrayListNative<Double>> dirList, NN<Data::ArrayListNative<Bool>> reverseList)
{
	NN<Math::Geometry::LineString> ls;
	NN<Math::Geometry::LineString> nextLS;
	UnsafeArray<const Math::Coord2DDbl> thisPoints;
	UnsafeArray<const Math::Coord2DDbl> nextPoints;
	Math::Coord2DDbl thisEndPt;
	Bool isReverse = false;
	Double dir1;
	Double dir2;
	UIntOS k;
	UIntOS l;
	UIntOS i = 0;
	UIntOS j = lineList->GetCount();
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
			Double dir = dir2 - dir1;
			if (dir < 0)
			{
				dir += 360;
			}
			dirList->Add(dir);
		}
		i++;
	}
}
